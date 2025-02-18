//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>

#include "RelativePointer.h"

#define IPTR_TO_MPTR &**
#define IPTR_TO_TPTR &***
#define ITR_TO_M *
#define ITR_TO_T **

namespace gE
{
	template<class T> class Manager;
	template<class T> class LinkedList;
	template<class T> class LinkedNode;
	template<class T> class LinkedIterator;

	template<class A_T, class B_T>
	using CompareFunc = bool(*)(const A_T& a, const B_T& b);

	enum class Direction : u8
	{
		Left,
		Right
	};

	template<class T>
	class LinkedList
	{
	 public:
		LinkedList() = default;

		DEFAULT_OPERATOR_MOVE(LinkedList);
		DELETE_OPERATOR_COPY(LinkedList);

		typedef LinkedNode<T> ITER_T;

		void Add(LinkedNode<T>& t, Direction = Direction::Right);
		void Insert(LinkedNode<T>& t, LinkedNode<T>* at, Direction = Direction::Right);
		void Insert(LinkedNode<T>& t, LinkedNode<T>& at, Direction = Direction::Right);
		void Move(LinkedNode<T>& t, LinkedNode<T>& to, Direction = Direction::Right);
		void Remove(LinkedNode<T>& t);
		void MergeList(LinkedList& from, ITER_T* begin = nullptr, ITER_T* end = nullptr);
		void EmptyUnsafe();
		void Empty();

		NODISCARD LinkedNode<T>* At(u32 index);
		NODISCARD ALWAYS_INLINE LinkedNode<T>* operator[](u32 i) { return At(i); }

		template<class COMP_T, CompareFunc<const COMP_T&, const T&> COMPARE_FUNC>
		LinkedNode<T>* FindSimilar(const COMP_T& similar, LinkedNode<T>* start = nullptr, Direction dir = Direction::Right, LinkedNode<T>* end = nullptr);

		GET_CONST(LinkedNode<T>*, First, _first);
		GET_CONST(LinkedNode<T>*, Last, _last);
		GET_CONST(u32, Size, _size);

		friend class LinkedNode<T>;

		LinkedIterator<T> begin() const { return _first; }
		LinkedIterator<T> end() const { return DEFAULT; }

		~LinkedList();

	 private:
		LinkedNode<T>* _first = nullptr;
		LinkedNode<T>* _last = nullptr;
		u32 _size = 0;
	};

	template<class T>
	class LinkedNode
	{
	public:
		LinkedNode() = default;
		explicit LinkedNode(T& owner) : _owner(owner) {};
		LinkedNode(T& owner, LinkedList<T>* l, LinkedNode* at = nullptr, Direction direction = Direction::Right);

		DELETE_OPERATOR_COPY(LinkedNode);
		OPERATOR_MOVE_NOSUPER(LinkedNode, Free,
			if(!o._list) return;

			_owner = o._owner;
			_list = o._list;
			_previous = o._previous;
			_next = o._next;
			if(_previous) _previous->_next = this;
			else _list->_first = this;
			if(_next) _next->_previous = this;
			else _list->_last = this;
			o._list = nullptr;
			o._previous = nullptr;
			o._next = nullptr;
		);

		GET_CONST(LinkedNode*, Previous, _previous);
		GET_CONST(LinkedNode*, Next, _next);
		GET_CONST(T&,, *_owner);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _owner; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _owner; }

		NODISCARD ALWAYS_INLINE T& operator*() { return *_owner; }
		NODISCARD ALWAYS_INLINE const T& operator*() const { return *_owner; }

		NODISCARD ALWAYS_INLINE bool IsValid() { return (_next || _previous) && _list;}

		void Move(LinkedNode& to, Direction direction = Direction::Right)
		{
			PlacementNew(*this, _list, _owner, &to, direction);
		}

		friend class LinkedList<T>;

		~LinkedNode() { Free(); }

	private:
		LinkedList<T>* _list = nullptr;
		LinkedNode* _previous = nullptr;
		LinkedNode* _next = nullptr;
		RelativePointer<T> _owner;

		void Free();
	};

	template<class T>
	class Manager
	{
	public:
		Manager() = default;

		DELETE_OPERATOR_CM(Manager);

		typedef LinkedNode<T> ITER_T;

		GET_CONST(const LinkedList<T>&, List, List);

		friend T;

		virtual ~Manager() = default;

	protected:
		virtual void OnRegister(T& t) { List.Add(t.GetNode()); };
		virtual void OnRemove(T& t) { List.Remove(t.GetNode()); };

		LinkedList<T> List;
	};

	template<class T>
	class LinkedIterator
	{
	public:
		LinkedIterator(LinkedNode<T>* node) : _node(node) {};
		LinkedIterator() = default;

		T* operator*() { return &**_node; }

		LinkedIterator& operator++() { _node = _node->GetNext(); return *this; }
		bool operator==(const LinkedIterator& b) { return _node == b._node; }
		bool operator!=(const LinkedIterator& b) { return _node != b._node; }

	private:
		LinkedNode<T>* _node = nullptr;
	};

	template<class T>
	class Managed
	{
	 public:
		Managed() = default;
		Managed(Manager<Managed>* m, T& t, bool overrideRegister = false) : Node(*this), _t(&t), _manager(m)
		{
			if(!overrideRegister) Register();
		}

		DELETE_OPERATOR_COPY(Managed);
		OPERATOR_MOVE_NOSUPER(Managed, Free,
			_t = o._t;
			Node = move(o.Iterator);
		);

		typedef LinkedNode<Managed> ITER_T;

		GET(Manager<Managed>, Manager, _manager);
		GET_CONST(ITER_T*, Previous, Node.GetPrevious());
		GET_CONST(ITER_T*, Next, Node.GetNext());
		GET_CONST(LinkedList<T>*, List, Node._list);
		GET(ITER_T&, Node, Node);
		GET_CONST(T&,, *_t);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _t; }

		NODISCARD ALWAYS_INLINE T& operator*() { return *_t; }
		NODISCARD ALWAYS_INLINE const T& operator*() const { return *_t; }

		friend class Manager<T>;

		virtual ~Managed() { Free(); }

	protected:
		ALWAYS_INLINE void Register();

		LinkedNode<Managed> Node;

	private:
		RelativePointer<T> _t;
		Manager<Managed>* _manager = nullptr;
#ifdef DEBUG
		bool _init = false;
#endif

		void Free();
	};
}

#include "Manager.inl"