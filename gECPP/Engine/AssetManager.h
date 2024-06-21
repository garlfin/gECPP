//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/GL.h"
#include "GL/Binary/Binary.h"

namespace gE
{
	template<class T>
	class Reference
	{
	 public:
		explicit inline Reference(T* t) : _t(t) { if(t) _counter = new u32(1); }
		inline Reference() = default;

		Reference(Reference&& o) noexcept : _t(o._t), _counter(o._counter)
		{
			o._t = nullptr;
			o._counter = nullptr;
		}

		Reference(const Reference& o) : _t(o._t), _counter(o._counter) { if(_counter) (*_counter)++; }

		OPERATOR_EQUALS_BOTH(Reference);

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t ?: t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t ?: t; }

		explicit ALWAYS_INLINE operator bool() const { return _t; }
		explicit ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		template<class I>
		ALWAYS_INLINE operator Reference<I>()
		{
			static_assert(std::is_base_of_v<I, T>);
			return Reference<I>(_t, _counter);
		}

		~Reference()
		{
			if(!_t || --(*_counter)) return;

			delete _t;
			delete _counter;
		}

	 private:
		Reference(T* t, u32* c) : _t(t), _counter(c) { if(_counter) (*_counter)++; }

		T* _t = nullptr;
		u32* _counter = nullptr;

		template<class I> friend class Reference;
	};

	template<class T, typename... ARGS> requires requires(ARGS&&... a) { T(std::forward<ARGS>(a)...); }
	ALWAYS_INLINE Reference<T> ref_create(ARGS&& ... args)
	{
		return Reference<T>(new T(std::forward<ARGS>(args)...));
	}

	/// Gives ownership of the pointer to the Reference.
	template<class T>
	ALWAYS_INLINE Reference<T> ref_cast(T* t)
	{
		return Reference<T>(t);
	}

	template<class T>
	class SmartPointer
	{
	 public:
		ALWAYS_INLINE explicit SmartPointer(T* t) : _t(t) { };
		SmartPointer() = default;

		SmartPointer(const SmartPointer&) = delete;
		ALWAYS_INLINE SmartPointer(SmartPointer&& o) noexcept : _t(o.Release()) { }

		OPERATOR_EQUALS_XVAL(SmartPointer);
		SmartPointer& operator=(const SmartPointer&) = delete;

		ALWAYS_INLINE T* Get() const { return _t; }
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t ?: t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t ?: t; }

		explicit ALWAYS_INLINE operator bool() const { return _t; }
		explicit ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		template<class O>
		ALWAYS_INLINE SmartPointer<O> Move()
		{
			static_assert(std::is_base_of_v<O, T>);
			return SmartPointer<O>(Release());
		}

		T* Release() { T* t = _t; _t = nullptr; return t; }

		~SmartPointer() { delete _t; }

	 private:
		T* _t = nullptr;

		template<class I> friend class SmartPointer;
	};

	template<typename T, typename... ARGS> requires requires(ARGS&&... a) { T(std::forward<ARGS>(a)...); }
	ALWAYS_INLINE SmartPointer<T> ptr_create(ARGS&&... args)
	{
		return SmartPointer<T>(new T(std::forward<ARGS>(args)...));
	}

	/// Gives ownership of the pointer to the SmartPointer.
	template<typename T>
	ALWAYS_INLINE SmartPointer<T> ptr_cast(T* t)
	{
		return SmartPointer<T>(t);
	}
}
