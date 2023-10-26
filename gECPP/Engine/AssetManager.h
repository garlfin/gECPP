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
	class Reference
	{
	 public:
		inline explicit Reference(T* t) : _t(t), _counter(new u32(1)) {};
		Reference(Reference&& o) noexcept : _t(o._t), _counter(o._counter) { o._t = o._counter = nullptr; }
		Reference(const Reference& o) : _t(o._t), _counter(o._counter) { if(_counter) (*_counter)++;}
		inline Reference() = default;

		OPERATOR_EQUALS_BOTH(Reference);

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator T*() const { return _t; } // NOLINT
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

		~Reference()
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
	Reference<T> CreateReference(ARGS&&... args) { return Reference<T>(new T(args...)); }

	/// Gives ownership of the pointer to the Reference.
	template<class T>
	Reference<T> CreateReferenceFromPointer(T* t) { return Reference<T> (t); }

	template<class T>
	class SmartPointer
	{
	 public:
		SmartPointer() = default;
		explicit SmartPointer(T* t) : _t(t) {};

		SmartPointer(SmartPointer&& o) noexcept : _t(o._t) { o._t = nullptr; }
		SmartPointer& operator=(SmartPointer&& o) noexcept
		{
			if(this == &o) return *this;

			_t = o._t;
			o._t = nullptr;

			return *this;
		}

		SmartPointer(const SmartPointer&) = delete;
		SmartPointer& operator=(const SmartPointer&) = delete;

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator T*() const { return _t; } // NOLINT
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

		~SmartPointer() { delete _t; }

	 private:
			T* _t = nullptr;
	};

	template<typename T, typename... ARGS>
	SmartPointer<T> CreateSmartPointer(ARGS&&... args) { return SmartPointer<T>(new T(args...)); }

	/// Gives ownership of the pointer to the SmartPointer.
	template<typename T>
	SmartPointer<T> CreateSmartPointerFromPointer(T* t) { return SmartPointer<T>(t); }
}
