//
// Created by scion on 9/26/2024.
//

#pragma once

#include "Manager.h"

namespace gE
{
	template<class T>
	void LinkedList<T>::Add(LinkedIterator<T>& t)
	{
		t = MOVE(ITER_T(*this, t.Get()));
	}

	template<class T>
	void LinkedList<T>::Remove(LinkedIterator<T>& t)
	{
		t = MOVE(ITER_T(*this, t.Get()));
	}

	template<class T>
	void LinkedList<T>::Move(LinkedIterator<T>& t, LinkedIterator<T>& to)
	{
		t = MOVE(LinkedIterator<T>(*this, t.Get(), &to));
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedIterator<T>& t, LinkedIterator<T>& at)
	{
		t = MOVE(LinkedIterator<T>(*this, t.Get(), &at));
	}

	template<class T>
	void LinkedList<T>::MergeList(LinkedList& list)
	{
		if(!list._size) return;

		for(ITER_T* i = list._first; i; i = i->_next)
			i->_list = this;

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
	template<CompareFunc<const T&> COMPARE_FUNC>
	LinkedIterator<T>* LinkedList<T>::FindSimilar(const T& similar, gE::LinkedIterator<T>* start, SearchDirection direction)
	{
		if(!start)
			start = direction == SearchDirection::Right ? _first : _last;

		if(start._manager != this) return nullptr;

		for(LinkedIterator<T>* c = start; c; c = direction == SearchDirection::Left ? c->GetPrevious() : c->GetNext())
			if(COMPARE_FUNC(similar, *c)) return c;

		return nullptr;
	}

	template<class OWNER_T>
	LinkedList<OWNER_T>::~LinkedList()
	{
		for(LinkedIterator<OWNER_T>* m = _first; m; m = m->_next)
			m->_list = nullptr;

		_size = 0;
		_first = _last = nullptr;
	}
}
