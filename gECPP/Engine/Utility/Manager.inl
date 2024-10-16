//
// Created by scion on 9/26/2024.
//

#pragma once

#include "Manager.h"

namespace gE
{
	template<class T>
	void LinkedList<T>::Add(LinkedIterator<T>& t, Direction dir)
	{
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this, nullptr, dir);
	}

	template<class T>
	void LinkedList<T>::Remove(LinkedIterator<T>& t)
	{
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this);
	}

	template<class T>
	void LinkedList<T>::Move(LinkedIterator<T>& t, LinkedIterator<T>& to, Direction dir)
	{
		Insert(t, &to, dir);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedIterator<T>& t, LinkedIterator<T>& at, Direction dir)
	{
		Insert(t, &at, dir);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedIterator<T>& t, LinkedIterator<T>* at, Direction dir)
	{
		SAFE_CONSTRUCT(t, ITER_T, t.Get(), *this, at, dir);
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
	LinkedIterator<T>* LinkedList<T>::At(u32 index)
	{
		ITER_T* t = _first;
		for(u32 i = 0; i < index; i++)
			t = t->_next;
		return t;
	}

	template<class T>
	template<class COMP_T, CompareFunc<const COMP_T&, const T&> COMPARE_FUNC>
	LinkedIterator<T>* LinkedList<T>::FindSimilar(const COMP_T& similar, LinkedIterator<T>* start, Direction dir, LinkedIterator<T>* end)
	{
		if(!_first) return nullptr;

		if(!start)
			start = dir == Direction::Right ? _first : _last;

		if(start->_list != this) return nullptr;
		if(end && end->_list != this) return nullptr;

		for(LinkedIterator<T>* c = start; c && c != end; c = dir == Direction::Left ? c->GetPrevious() : c->GetNext())
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
	LinkedIterator<T>::LinkedIterator(T& owner, LinkedList<T>& l, LinkedIterator* at, Direction direction):
		_list(&l), _owner(owner)
	{
		_list->_size++;

		if(!_list->_first)
		{
			_next = _previous = nullptr;
			_list->_first = _list->_last = this;
			return;
		}

		if(direction == Direction::Right)
		{
			if(!at) at = _list->_last;

			_next = at->_next;
			_previous = at;

			_previous->_next = this;

			if(_next) _next->_previous = this;
			else _list->_last = this;
		}
		else
		{
			if(!at) at = _list->_first;

			_next = at;
			_previous = at->_previous;

			if(_previous) _previous->_next = this;
			else _list->_first = this;

			_next->_previous = this;
		}
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
