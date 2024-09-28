//
// Created by scion on 9/7/2023.
//

#pragma once

#include <gECPP/Prototype.h>
#include <gECPP/Engine/Utility/Macro.h>

#include "RelativePointer.h"

namespace gE
{
	template<class T> class Manager;
	template<class T> class ManagedList;

	template<class T>
	using CompareFunc = bool(*)(T a, T b);

	enum class SearchDirection : u8
	{
		Left,
		Right
	};

	template<class T>
	class ManagedList
	{
	 public:
		ManagedList(Manager<T>* = nullptr);

		DELETE_OPERATOR_CM(ManagedList);

		void Add(Managed<T>& t);
		void Insert(Managed<T>& t, Managed<T>& at);
		void Move(Managed<T>& t, Managed<T>& to);
		void Remove(Managed<T>& t);
		void MergeList(ManagedList& list);

		template<CompareFunc<const T&> COMPARE_FUNC>
		ALWAYS_INLINE Managed<T>* FindSimilar(Managed<T>& similar)
		{
			return FindSimilar(similar, _first, SearchDirection::Right);
		}

		template<CompareFunc<const T&> COMPARE_FUNC>
		Managed<T>* FindSimilar(Managed<T>& similar, Managed<T>& start, SearchDirection direction);

		GET_CONST(Managed<T>*, First, _first);
		GET_CONST(Managed<T>*, Last, _last);
		GET_CONST(u32, Size, _size);

	 private:
		Managed<T>* _first = nullptr;
		Managed<T>* _last = nullptr;
		u32 _size = 0;
	};

	template<class T>
	class Manager
	{
	 public:
		virtual ~Manager() = default;

		Manager() = default;

		DELETE_OPERATOR_CM(Manager);

		virtual void OnUpdate(float d) = 0;
		virtual void OnRender(float d, Camera* camera) = 0;

		inline void Register(T& t) { List.Remove(t); }
		inline void Remove(T& t) { List.Add(t); };

	 protected:
		virtual void OnRegister(T&) {};
		virtual void OnRemove(T&) {};

		ManagedList<T> List;
	};

	template<class T>
	struct ManagedIterator
	{
	public:
		ManagedIterator() = default;
		ManagedIterator(ManagedList<T>& l, Managed<T>& t, ManagedIterator* at, SearchDirection direction = SearchDirection::Right) :
			_t(t)
		{
			if(!at) return;

			if(direction == SearchDirection::Right)
			{
				_next = at->_next;
				_previous = at;

				if(_next) _next->_previous = this;
				at->_next = this;
			} else
			{
				_previous = at->_previous;
				_next = at;

				if(_previous) _previous->_next = this;
				at->_previous = this;
			}
		}

		DELETE_OPERATOR_COPY(ManagedIterator);
		OPERATOR_MOVE(ManagedIterator, o,
			_t = o._t;
			_previous = o._previous;
			_next = o._next;

			if(_previous) _previous->_next = this;
			if(_next) _next->_previous = this;

			o._previous = nullptr;
			o._next = nullptr;
		);

		GET_CONST(ManagedIterator*, Previous, _previous);
		GET_CONST(ManagedIterator*, Next, _next);
		GET_CONST(T&,, *_t);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _t; }

		~ManagedIterator()
		{
			if(_previous) _previous->_next = _next;
			if(_next) _next->_previous = _previous;
		}

	private:
		ManagedList<T>* _list;
		ManagedIterator* _previous = nullptr;
		ManagedIterator* _next = nullptr;
		RelativePointer<Managed<T>> _t;
	};

	template<class T>
	class Managed
	{
	 public:
		explicit inline Managed(T& t, ManagedList<T>* m = nullptr) : _iterator(), _t(&t)
		{
		}

		OPERATOR_COPY(Managed, o,
			_t = o._t;
			_iterator = o._iterator;
		);

		OPERATOR_MOVE(Managed, o,
			_t = o._t;
			_iterator = MOVE(_iterator);
		);

		GET_CONST(Managed*, Previous, _iterator._previous);
		GET_CONST(Managed*, Next, _iterator._next);
		GET_CONST(ManagedList<T>*, List, _list);
		GET_CONST(T&,, *_t);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _t; }

		virtual ~Managed() = default;

	 private:
		ManagedList<T>* _list = nullptr;
		ManagedIterator<T> _iterator;
		RelativePointer<T> _t;
	};
}

#include "Manager.inl"