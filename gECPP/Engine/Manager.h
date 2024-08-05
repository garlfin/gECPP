//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <vector>
#include <algorithm>
#include <Engine/Binary/Macro.h>
#include <Engine/Binary/Binary.h>

namespace gE
{
	template<class T>
	class ManagedList
	{
	 public:
		ManagedList() = default;

		DELETE_COPY_CONSTRUCTOR(ManagedList);

		void Add(Managed<T>& t);
		void Insert(Managed<T>& t, Managed<T>& at);
		void Remove(Managed<T>& t);
		void MoveFrom(ManagedList<T>& l);

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
		Manager() = default;

		DELETE_COPY_CONSTRUCTOR(Manager);

		virtual void OnUpdate(float d) = 0;
		virtual void OnRender(float d, Camera* camera) = 0;

		friend class Managed<T>;

	 protected:
		virtual void Register(T& t)
		{
			List.Add(t);
		}

		virtual void Remove(T& t)
		{
			List.Remove(t);
		}

		ManagedList<T> List;
	};

	template<class T>
	class Managed
	{
	 public:
		explicit inline Managed(T& t, Manager<T>* m = nullptr) : _manager(m), _t(t)
		{
			if(m) m->Register(t);
		}

		DELETE_COPY_CONSTRUCTOR(Managed);

		GET_CONST(Managed<T>*, Previous, _previous);
		GET_CONST(Managed<T>*, Next, _next);
		GET_CONST(ManagedList<T>*, List, _list);
		GET_CONST(Manager<T>*, Manager, _manager);
		GET_CONST(T&,, _t);

		NODISCARD ALWAYS_INLINE T* operator->() { return &_t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return &_t; }

		inline ~Managed() { if(_manager) _manager->Remove(_t); }

	 private:
		ManagedList<T>* _list = nullptr;
		Manager<T>* _manager;

		Managed<T>* _previous = nullptr;
		T& _t;
		Managed<T>* _next = nullptr;

		friend class ManagedList<T>;
	};

	template<class T>
	void ManagedList<T>::Add(Managed<T>& t)
	{
		if(t._list || t._previous || t._next) return;

		if(!_size++)
		{
			_first = _last = &t;
			t._previous = t._next = nullptr;
			return;
		}

		t->_previous = _last;
		t->_list = this;
		_last = _last->_next = &t;
	}

	template<class T>
	void ManagedList<T>::Remove(Managed<T>& t)
	{
		if(t._list != this) return;

		if(!--_size) _first = _last = nullptr;

		if(t._previous) t._previous->_next = t._next;
		if(t._next) t._next->_previous = t._previous;

		if(_first == &t) _first = t._next;
		else if(_last == &t) _last = t._previous;

		t._previous = t._next = nullptr;
		t._list = nullptr;
	}

	template<class T>
	void ManagedList<T>::Insert(Managed<T>& t, Managed<T>& at)
	{
		if(at._list != this) return;
		if(t._list || t._previous || t._next) return;

		t._previous = &at;
		t._next = at._next;
		at._next = &t;

		if(_last == &at) _last = &t;
	}

	template<class T>
	void ManagedList<T>::MoveFrom(ManagedList<T>& list)
	{
		if(!list._size) return;

		for(Managed<T>* m = list._first; m; m = m->_next)
			m->_list = this;

		if (_size)
		{
			list._first->_previous = _first;
			_last->_next = list._first;
		}
		else _first = list._first;

		_size += list._size;
		_last = list._last;

		list._first = list._last = nullptr;
		list._size = 0;
	}
}