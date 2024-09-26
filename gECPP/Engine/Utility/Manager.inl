//
// Created by scion on 9/26/2024.
//

#pragma once

#include "Manager.h"

namespace gE
{
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
	void ManagedList<T>::MergeList(ManagedList<T>& list)
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

	template<class T>
	void ManagedList<T>::Move(Managed<T>& t, Managed<T>& to)
	{
		if(t._previous) t._previous->_next = t._next;
		else _first = t._next;

		if(t._next) t._next->_previous = t._previous;
		else _last = t._previous;

		t._previous = &to;
		t._next = to._next;

		to._next = &t;
	}

	template<class T>
	template<CompareFunc<const T&> COMPARE_FUNC>
	Managed<T>* ManagedList<T>::FindSimilar(Managed<T>& similar, Managed<T>& start, SearchDirection direction)
	{
		if(start._manager != this) return nullptr;

		for(Managed<T>* c = &start; c; c = direction == SearchDirection::Left ? c->GetPrevious() : c->GetNext())
			if(COMPARE_FUNC(*similar, *c)) return c;

		return nullptr;
	}
}
