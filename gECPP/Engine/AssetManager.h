//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/gl.h"

#define CONSTRUCTOR(TYPE, REFTYPE, MODIFIER) \
	TYPE& operator=(Handle REFTYPE o) MODIFIER \
	{ \
		if(&o == this) return *this; \
		this->~TYPE(); \
		new(this) TYPE(o); \
		return *this; \
	}

namespace gE
{
	class Window;

	template<class T>
	struct Handle
	{
		inline Handle(T* t) : _t(t), _counter(new u32(1)) {}
		inline Handle(const Handle& h) : _t(h._t), _counter(h._counter) { ++(*_counter); }
		inline Handle(Handle&& h) noexcept : _t(h._t), _counter(h._counter) { h._counter = nullptr; h._t = nullptr; }
		inline Handle() = default;

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }

		template<typename... ARGS>
		inline static Handle Create(ARGS&&... args) { return Handle(new T(args...)); }

		CONSTRUCTOR(Handle, const &,);
		CONSTRUCTOR(Handle, &&, noexcept);

		~Handle()
		{
			if(!_t || --(*_counter)) return;

			delete _t;
			delete _counter;
		}

	 private:
		T* _t;
		u32* _counter;
	};

	template<class T, typename... ARGS>
	Handle<T> CreateHandle(ARGS&&... args)
	{
		return Handle<T>::Create(std::forward<ARGS>(args)...);
	}

	template<class T>
	Handle<T> CreateHandleFromPointer(T* t)
	{
		return Handle<T>(t);
	}
}
