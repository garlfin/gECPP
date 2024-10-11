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
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this);
	}

	template<class T>
	void LinkedList<T>::Remove(LinkedIterator<T>& t)
	{
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this);
	}

	template<class T>
	void LinkedList<T>::Move(LinkedIterator<T>& t, LinkedIterator<T>& to)
	{
		Insert(t, &to);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedIterator<T>& t, LinkedIterator<T>& at)
	{
		Insert(t, &at);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedIterator<T>& t, LinkedIterator<T>* at)
	{
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this, at);
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
	template<class COMP_T, CompareFunc<const COMP_T&, const T&> COMPARE_FUNC>
	LinkedIterator<T>* LinkedList<T>::FindSimilar(const COMP_T& similar, LinkedIterator<T>* start, SearchDirection dir, LinkedIterator<T>* end)
	{
		if(!_first) return nullptr;

		if(!start)
			start = dir == SearchDirection::Right ? _first : _last;

		if(start->_list != this) return nullptr;
		if(end && end->_list != this) return nullptr;

		for(LinkedIterator<T>* c = start; c && c != end; c = dir == SearchDirection::Left ? c->GetPrevious() : c->GetNext())
			if(COMPARE_FUNC(similar, **c)) return c;

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

	template<class T>
	Managed<T>::~Managed()
	{
		if(!_manager) return;

		_manager->OnRemove(*this);
	#ifdef DEBUG
		GE_ASSERT(_init, "UNINITIALIZED MANAGED OBJECT!");
	#endif
	}

	template<class T>
	void Managed<T>::Register()
	{
		if(!_manager) return;

		_manager->OnRegister(*this);
	#ifdef DEBUG
		GE_ASSERT(!_init, "DOUBLE INITIALIZED MANAGED OBJECT!");
		_init = true;
	#endif
	}
}
