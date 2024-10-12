//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <Engine/Utility/Macro.h>

#include "RelativePointer.h"

#define IPTR_TO_MPTR &**
#define IPTR_TO_TPTR &***
#define ITR_TO_M *
#define ITR_TO_T **

namespace gE
{
	template<class T> class Manager;
	template<class T> class LinkedList;
	template<class T> class LinkedIterator;

	template<class A_T, class B_T>
	using CompareFunc = bool(*)(const A_T& a, const B_T& b);

	enum class SearchDirection : u8
	{
		Left,
		Right
	};

	template<class T>
	class LinkedList final
	{
	 public:
		LinkedList() = default;

		DEFAULT_OPERATOR_MOVE(LinkedList);
		DELETE_OPERATOR_COPY(LinkedList);

		typedef LinkedIterator<T> ITER_T;

		void Add(LinkedIterator<T>& t);
		void Insert(LinkedIterator<T>& t, LinkedIterator<T>* at);
		void Insert(LinkedIterator<T>& t, LinkedIterator<T>& at);
		void Move(LinkedIterator<T>& t, LinkedIterator<T>& to);
		void Remove(LinkedIterator<T>& t);
		void MergeList(LinkedList& list);

		NODISCARD LinkedIterator<T>* At(u32 index);
		NODISCARD ALWAYS_INLINE LinkedIterator<T>* operator[](u32 i) { return At(i); }

		template<class COMP_T, CompareFunc<const COMP_T&, const T&> COMPARE_FUNC>
		LinkedIterator<T>* FindSimilar(const COMP_T& similar, LinkedIterator<T>* start = nullptr, SearchDirection dir = SearchDirection::Right, LinkedIterator<T>* end = nullptr);

		GET_CONST(LinkedIterator<T>*, First, _first);
		GET_CONST(LinkedIterator<T>*, Last, _last);
		GET_CONST(u32, Size, _size);

		friend class LinkedIterator<T>;

		~LinkedList();

	 private:
		LinkedIterator<T>* _first = nullptr;
		LinkedIterator<T>* _last = nullptr;
		u32 _size = 0;
	};

	template<class T>
	class LinkedIterator
	{
	public:
		LinkedIterator() = default;
		explicit LinkedIterator(T& owner) : _owner(owner) {};
		LinkedIterator(T& owner, LinkedList<T>& l, LinkedIterator* at = nullptr, SearchDirection direction = SearchDirection::Right);

		DELETE_OPERATOR_COPY(LinkedIterator);
		OPERATOR_MOVE(LinkedIterator, o,
			if(o._list)
			{
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
			};
		);

		GET_CONST(LinkedIterator*, Previous, _previous);
		GET_CONST(LinkedIterator*, Next, _next);
		GET_CONST(T&,, *_owner);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _owner; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _owner; }

		NODISCARD ALWAYS_INLINE T& operator*() { return *_owner; }
		NODISCARD ALWAYS_INLINE const T& operator*() const { return *_owner; }

		NODISCARD ALWAYS_INLINE bool IsValid() { return (_next || _previous) && _list;}

		void Move(LinkedIterator& to, SearchDirection direction = SearchDirection::Right)
		{
			SAFE_CONSTRUCT(*this, LinkedIterator, _list, _owner, &to, direction);
		}

		friend class LinkedList<T>;

		~LinkedIterator()
		{
			if(!_list) return;

			if(_previous) _previous->_next = _next;
			else _list->_first = _next;

			if(_next) _next->_previous = _previous;
			else _list->_last = _previous;

			_list->_size--;

			_list = nullptr;
		}

	private:
		LinkedList<T>* _list = nullptr;
		LinkedIterator* _previous = nullptr;
		LinkedIterator* _next = nullptr;
		RelativePointer<T> _owner;
	};

	template<class T>
	class Manager
	{
	public:
		Manager() = default;

		DELETE_OPERATOR_CM(Manager);

		typedef LinkedIterator<T> ITER_T;

		virtual void OnUpdate(float d) = 0;
		virtual void OnRender(float d, Camera* camera) = 0;

		friend T;

		virtual ~Manager() = default;

	protected:
		virtual void OnRegister(T& t) { List.Add(t.GetIterator()); };
		virtual void OnRemove(T& t) { List.Remove(t.GetIterator()); };

		LinkedList<T> List;
	};

	template<class T>
	class Managed
	{
	 public:
		Managed() = default;
		Managed(Manager<Managed>* m, T& t, bool overrideRegister = false) : Iterator(t), _t(&t), _manager(m)
		{
			if(!overrideRegister) Register();
		}

		DELETE_OPERATOR_COPY(Managed);
		OPERATOR_MOVE(Managed, o,
			_t = o._t;
			Iterator = move(o.Iterator);
		);

		typedef LinkedIterator<Managed> ITER_T;

		GET(Manager<Managed>, Manager, _manager);
		GET_CONST(Managed*, Previous, Iterator._previous);
		GET_CONST(Managed*, Next, Iterator._next);
		GET_CONST(LinkedList<T>*, List, Iterator._list);
		GET(ITER_T&, Iterator, Iterator);
		GET_CONST(T&,, *_t);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _t; }

		NODISCARD ALWAYS_INLINE T& operator*() { return *_t; }
		NODISCARD ALWAYS_INLINE const T& operator*() const { return *_t; }

		friend class Manager<T>;

		virtual ~Managed();

	protected:
		ALWAYS_INLINE void Register();

		LinkedIterator<Managed> Iterator;

	private:
		RelativePointer<T> _t;
		Manager<Managed>* _manager = nullptr;
#ifdef DEBUG
		bool _init = false;
#endif
	};
}

#include "Manager.inl"