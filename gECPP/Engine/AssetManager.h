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
		inline Reference(T* t) : _t(t) { if(t) _counter = new u32(1); }

		template<class I>
		explicit Reference(Reference<I>&& o) noexcept : _t(o._t), _counter(o._counter)
		{
			static_assert(std::is_convertible_v<I, T> || std::is_base_of_v<T, I>);
			o._t = nullptr;
			o._counter = nullptr;
		}
		template<class I>
		explicit Reference(const Reference<I>& o) : _t(o._t), _counter(o._counter)
		{
			static_assert(std::is_convertible_v<I, T> || std::is_base_of_v<T, I>);
			if(_counter) (*_counter)++;
		}

		inline Reference() = default;

		template<class I>
		OPERATOR_EQUALS_T(Reference, Reference<I>);

		template<class I>
		OPERATOR_EQUALS_XVAL_T(Reference, Reference<I>);

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

		template<class I>
		friend class Reference;
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

		SmartPointer(const SmartPointer&) = delete;
		template<class I>
		explicit SmartPointer(SmartPointer&& o) noexcept : _t(o._t)
		{
			static_assert(std::is_convertible_v<I, T> || std::is_base_of_v<T, I>);
			o._t = nullptr;
			o._counter = nullptr;
		}

		template<class I>
		OPERATOR_EQUALS_XVAL_T(SmartPointer, SmartPointer<I>);
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

		template<class I>
		friend class SmartPointer;
	};

	template<typename T, typename... ARGS>
	SmartPointer<T> CreateSmartPointer(ARGS&&... args) { return SmartPointer<T>(new T(args...)); }

	/// Gives ownership of the pointer to the SmartPointer.
	template<typename T>
	SmartPointer<T> CreateSmartPointerFromPointer(T* t) { return SmartPointer<T>(t); }
}
