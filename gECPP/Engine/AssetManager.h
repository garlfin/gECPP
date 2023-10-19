//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/GL.h"
#include "GL/Binary.h"

namespace gE
{
	template<class T>
	class Handle
	{
	 public:
		inline explicit Handle(T* t) : _t(t), _counter(new u32(1)) {};
		Handle(Handle&& o) noexcept : _t(o._t), _counter(o._counter) { o._t = o._counter = nullptr; }
		Handle(const Handle& o) : _t(o._t), _counter(o._counter) { if(_counter) (*_counter)++;}
		inline Handle() = default;

		OPERATOR_EQUALS_BOTH(Handle);

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

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
	Handle<T> CreateHandle(ARGS&&... args) { return Handle<T>::Create(std::forward<ARGS>(args)...); }

	/// Gives ownership of the pointer to the Handle.
	template<class T>
	Handle<T> CreateHandleFromPointer(T* t) { return Handle<T>(t); }

	/// Literally a unique pointer...
	template<class T>
	class Reference
	{
	 public:
		Reference() = default;
		Reference(T* t) : _t(t) {}; // NOLINT

		Reference(Reference&& o) noexcept : _t(o._t) { o._t = nullptr; }
		Reference& operator=(Reference&& o) noexcept
		{
			if(this == &o) return *this;

			_t = o._t;
			o._t = nullptr;

			return *this;
		}

		Reference(const Reference&) = delete;
		Reference& operator=(const Reference&) = delete;

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		~Reference() { delete _t; }

	 private:
			T* _t = nullptr;
	};

	template<typename T, typename... ARGS>
	Reference<T> CreateReference(ARGS&&... args) { return Reference<T>(new T(args...)); }

	template<typename T>
	Reference<T> CreateReferenceFromPointer(T* t) { return Reference<T>(t); }
}
