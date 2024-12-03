//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "Binary.h"
#include "Serializable/Asset.h"

namespace gE
{
	template<class T>
	class Reference : public Asset
	{
	 public:
		explicit inline Reference(T* t) : _t(t) { if(t) _counter = new u32(1); }
		inline Reference() = default;

		OPERATOR_COPY_NOSUPER(Reference,
			_t = o._t;
			_counter = o._counter;

			if(!_counter) return;

			LOG("INFO: COPIED REFERENCE\n\tCOUNT: " << *_counter << "\n\tFUNCTION: " << PRETTY_FUNCTION);
			(*_counter)++;
		);

		OPERATOR_MOVE_NOSUPER(Reference,
			_t = o._t;
			_counter = o._counter;
			o._t = nullptr;
			o._counter = nullptr;
		);

		GET(T&, , *_t);
		GET(T*, Pointer, _t);

		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT
		explicit ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t ? _t : t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t ? _t : t; }

		ALWAYS_INLINE bool operator == (const Reference& o) const { return _t == o._t; }
		ALWAYS_INLINE bool operator != (const Reference& o) const { return _t != o._t; }

		explicit ALWAYS_INLINE operator bool() const { return _t; }

		void Free() override
		{
			if(!_t || --(*_counter)) return;

			LOG("INFO: DELETED REFERENCE\n\tFunction: " << PRETTY_FUNCTION);
			delete _t;
			delete _counter;

			_t = nullptr;
			_counter = nullptr;
		}
		
		NODISCARD bool IsFree() const override { return !_t; }

		template<class I>
		ALWAYS_INLINE operator Reference<I>()
		{
			static_assert(std::is_base_of_v<I, T>);
			return Reference<I>(_t, _counter);
		}

		~Reference() override { Reference::Free(); }

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
	class SmartPointer : public Asset
	{
	 public:
		ALWAYS_INLINE explicit SmartPointer(T* t) : _t(t) { };
		SmartPointer() = default;

		DEFAULT_OPERATOR_COPY(SmartPointer);
		OPERATOR_MOVE_NOSUPER(SmartPointer, _t = o.Release())

		GET(T&, , *_t);
		GET(T*, Pointer, _t);

		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT
		explicit ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t ? _t : t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t ? _t : t; }

		ALWAYS_INLINE bool operator == (const SmartPointer& o) const { return _t == o._t; }
		ALWAYS_INLINE bool operator != (const SmartPointer& o) const { return _t != o._t; }

		explicit ALWAYS_INLINE operator bool() const { return _t; }

		void Free() override { delete _t; _t = nullptr; }
		NODISCARD bool IsFree() const override { return !_t; }

		template<class O>
		ALWAYS_INLINE SmartPointer<O> Move()
		{
			static_assert(std::is_base_of_v<O, T>);
			return SmartPointer<O>(Release());
		}

		T* Release() { T* t = _t; _t = nullptr; return t; }

		~SmartPointer() override { SmartPointer::Free(); }

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
