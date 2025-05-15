//
// Created by scion on 9/26/2024.
//

#pragma once

#include "Manager.h"

namespace gE
{
	template<class T>
	void LinkedList<T>::Add(LinkedNode<T>& t, Direction dir)
	{
		PlacementNew(t, t.Get(), this, nullptr, dir);
	}

	template<class T>
	void LinkedList<T>::Remove(LinkedNode<T>& t)
	{
		if(t._list != this) return;
		PlacementNew(t, t.Get(), nullptr);
	}

	template<class T>
	void LinkedList<T>::Move(LinkedNode<T>& t, LinkedNode<T>& to, Direction dir)
	{
		Insert(t, &to, dir);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedNode<T>& t, LinkedNode<T>& at, Direction dir)
	{
		Insert(t, &at, dir);
	}

	template<class T>
	void LinkedList<T>::Insert(LinkedNode<T>& t, LinkedNode<T>* at, Direction dir)
	{
		PlacementNew(t, t.Get(), this, at, dir);
	}

	template<class T>
	void LinkedList<T>::MergeList(LinkedList& from, ITER_T* begin, ITER_T* end)
	{
		const u32 previousSize = _size;

		if(!from._size) return;
		if(!begin) begin = from._first;
		if(!end) end = from._last;

		for(ITER_T* i = begin; i; i = i->_next)
		{
			i->_list = this;
			_size++;
			from._size--;
			if(i == end) break;
		}

		ITER_T* prevLast = _last;

		if(previousSize)
			_last->_next = begin;
		else
			{
				_first = begin;
				_last = end;
			}

		if(begin->_previous)
			begin->_previous->_next = end->_next;
		else
			from._first = end->_next;

		if(end->_next)
			end->_next->_previous = begin->_previous;
		else
			from._last = end->_previous;

		begin->_previous = prevLast;
		end->_next = nullptr;

		if(_size) GE_ASSERTM(_first && _last, "");
	}

	template <class T>
	void LinkedList<T>::EmptyUnsafe()
	{
		_size = 0;
		_first = _last = nullptr;
	}

	template <class T>
	void LinkedList<T>::Empty()
	{
		for(ITER_T* i = _first; i;)
		{
			ITER_T& toBeDeleted = *i;
			i = i->GetNext();
			PlacementNew(toBeDeleted);
		}
		_size = 0;
	}

	template<class T>
	LinkedNode<T>* LinkedList<T>::At(u32 index)
	{
		ITER_T* t = _first;
		for(u32 i = 0; i < index; i++)
			t = t->_next;
		return t;
	}

	template<class T>
	template<class COMP_T, CompareFunc<const COMP_T&, const T&> COMPARE_FUNC>
	LinkedNode<T>* LinkedList<T>::FindSimilar(const COMP_T& similar, LinkedNode<T>* start, Direction dir, LinkedNode<T>* end)
	{
		if(!_first) return nullptr;

		if(!start)
			start = dir == Direction::Right ? _first : _last;

		if(start->_list != this) return nullptr;
		if(end && end->_list != this) return nullptr;

		for(LinkedNode<T>* c = start; c && c != end; c = dir == Direction::Left ? c->GetPrevious() : c->GetNext())
			if(COMPARE_FUNC(similar, **c)) return c;

		return nullptr;
	}

	template<class OWNER_T>
	LinkedList<OWNER_T>::~LinkedList()
	{
		for(LinkedNode<OWNER_T>* m = _first; m; m = m->_next)
			m->_list = nullptr;

		_size = 0;
		_first = _last = nullptr;
	}

	template<class T>
	LinkedNode<T>::LinkedNode(T& owner, LinkedList<T>* l, LinkedNode* at, Direction direction):
		_list(l), _owner(owner)
	{
		if(!_list) return;

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

	template <class T>
	void LinkedNode<T>::Free()
	{
		if(!_list) return;

		if(_previous) _previous->_next = _next;
		else _list->_first = _next;

		if(_next) _next->_previous = _previous;
		else _list->_last = _previous;

		_list->_size--;

		_list = nullptr;
	}

	template<class T>
	void Managed<T>::Free()
	{
		if(!_manager) return;

		_manager->OnRemove(*this);
	#ifdef DEBUG
		GE_ASSERTM(_init, "UNINITIALIZED MANAGED OBJECT!");
	#endif
	}

	template <class T>
	void Managed<T>::SetManager(Manager<Managed>* manager)
	{
		if(_manager) _manager->OnRemove(*this);

		_manager = manager;
		Register();
	}

	template<class T>
	void Managed<T>::Register()
	{
		if(!_manager) return;

		_manager->OnRegister(*this);
	#ifdef DEBUG
		GE_ASSERTM(!_init, "DOUBLE INITIALIZED MANAGED OBJECT!");
		_init = true;
	#endif
	}
}
