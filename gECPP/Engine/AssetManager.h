//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/gl.h"
#include "GL/Binary.h"

namespace gE
{
	class Window;

	template<class T>
	struct Handle
	{
		inline Handle(T* t) : _t(t), _counter(new u32(1)) {}
		Handle(Handle&& o) noexcept : _t(o._t), _counter(o._counter) { o._t = o._counter = nullptr; }
		Handle(const Handle& o) : _t(o._t), _counter(o._counter) { if(_counter) (*_counter)++;}
		inline Handle() = default;

		COPY_CONSTRUCTOR_BOTH(Handle);

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }

		template<typename... ARGS>
		inline static Handle Create(ARGS&&... args) { return Handle(new T(args...)); }

		~Handle()
		{
			if(!_t || --(*_counter)) return;

			delete _t;
			delete _counter;
		}

	 private:
		T* _t = nullptr;
		u32* _counter = nullptr;
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
