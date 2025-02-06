//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "Binary.h"

namespace gE
{
	template<class T>
	class WeakReference;

	struct RefCounter
	{
	public:
		u32 RefCount = 0;
		u32 WeakCount = 0;
	};

	template<class T>
	class Reference
	{
	 public:
		explicit inline Reference(T* t) : _t(t) { if(t) _counter = new RefCounter(1, 0); }
		Reference() = default;

		OPERATOR_COPY_NOSUPER(Reference, Free,
			_t = o._t;
			_counter = o._counter;

			if(!_counter) return;

			LOG("INFO: COPIED REFERENCE\n\tCOUNT: {}\n\tFUNCTION: {}", _counter->RefCount, PRETTY_FUNCTION);
			++_counter->RefCount;
		);

		OPERATOR_MOVE_NOSUPER(Reference, Free,
			_t = o._t;
			_counter = o._counter;
			o._t = nullptr;
			o._counter = nullptr;
		);

		GET_CONST(T&, , *_t);
		GET_CONST(T*, Pointer, _t);

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

		void Free()
		{
			if(!_t) return;

			if(!--_counter->RefCount)
			{
				LOG("INFO: DELETED REFERENCE\n\tFunction: {}", PRETTY_FUNCTION);

				delete _t;
				if(!_counter->WeakCount)
				{
					LOG("INFO: DELETED COUNTER\n\tFunction: {}", PRETTY_FUNCTION);
					delete _counter;
				}
			}

			_t = nullptr;
			_counter = nullptr;
		}
		
		NODISCARD ALWAYS_INLINE bool IsFree() const { return !_t; }

		template<class I>
		ALWAYS_INLINE operator Reference<I>()
		{
			static_assert(std::is_base_of_v<I, T>);
			return Reference<I>(_t, _counter);
		}

		~Reference() { Free(); }

	 private:
		Reference(T* t, RefCounter* c) : _t(t), _counter(c) { if(_counter) ++_counter->RefCount; }

		T* _t = nullptr;
		RefCounter* _counter = nullptr;

		friend class WeakReference<T>;
		template<class I> friend class Reference;
	};

	template<class T>
	class WeakReference
	{
	public:
		WeakReference() = default;
		WeakReference(const Reference<T>& ref) : _t(ref._t), _counter(ref._counter) { if(_counter) _counter->WeakCount++; }

		OPERATOR_COPY_NOSUPER(WeakReference, Free,
			_t = o._t;
			_counter = o._counter;

			if(!_t) return;

			LOG("INFO: COPIED REFERENCE\n\tCOUNT: {}\n\tFUNCTION: {}", _counter->WeakCount, PRETTY_FUNCTION);
			++_counter->WeakCount;
		);

		OPERATOR_MOVE_NOSUPER(WeakReference, Free,
			_t = o._t;
			_counter = o._counter;
			o._t = nullptr;
			o._counter = nullptr;
		);

		NODISCARD ALWAYS_INLINE T& Get() { GE_ASSERT(_counter->RefCount); return *_t; }
		NODISCARD ALWAYS_INLINE const T& Get() const { GE_ASSERT(_counter->RefCount); return *_t; };

		GET(T*, Pointer, _counter->RefCount ? _t : nullptr);

		ALWAYS_INLINE T* operator->() const { return _counter->RefCount ? _t : nullptr; }
		ALWAYS_INLINE T& operator*() const { GE_ASSERT(_counter->RefCount); return *_t; }
		ALWAYS_INLINE operator T&() const { GE_ASSERT(_counter->RefCount); return *_t; } // NOLINT
		explicit ALWAYS_INLINE operator T*() const { return _counter->RefCount ? _t : nullptr; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t && _counter->RefCount ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t && _counter->RefCount ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t && _counter->RefCount ? _t : t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t && _counter->RefCount ? _t : t; }

		ALWAYS_INLINE bool operator == (const WeakReference& o) const { return _t == o._t; }
		ALWAYS_INLINE bool operator != (const WeakReference& o) const { return _t != o._t; }

		bool IsValid() const { return _counter && _counter->RefCount; }

		operator Reference<T>()
		{
			return Reference<T>(GetPointer(), IsValid() ? _counter : nullptr);
		}

		void Free()
		{
			if(!_t) return;
			LOG("INFO: DELETED WEAK REFERENCE\n\tFunction: {}", PRETTY_FUNCTION);

			if(!--_counter->WeakCount && !_counter->RefCount) delete _counter;

			_t = nullptr;
			_counter = nullptr;
		}

		NODISCARD ALWAYS_INLINE bool IsFree() const { return _t; }

	private:
		T* _t = nullptr;
		RefCounter* _counter = nullptr;
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
	class Pointer
	{
	 public:
		ALWAYS_INLINE explicit Pointer(T* t) : _t(t) { };
		Pointer() = default;

		DELETE_OPERATOR_COPY(Pointer);
		OPERATOR_MOVE_NOSUPER(Pointer, Free, _t = o.Release())

		GET_CONST(T&, , *_t);
		GET_CONST(T*, Pointer, _t);

		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT
		explicit ALWAYS_INLINE operator T*() const { return _t; } // NOLINT

		ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
		ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
		ALWAYS_INLINE const T* operator||(const T* t) const { return _t ? _t : t; };
		ALWAYS_INLINE T* operator||(T* t) const { return _t ? _t : t; }

		ALWAYS_INLINE bool operator == (const Pointer& o) const { return _t == o._t; }
		ALWAYS_INLINE bool operator != (const Pointer& o) const { return _t != o._t; }

		explicit ALWAYS_INLINE operator bool() const { return _t; }

		void Free() { delete _t; _t = nullptr; }
		NODISCARD bool IsFree() const { return !_t; }

		template<class O>
		ALWAYS_INLINE Pointer<O> Move()
		{
			static_assert(std::is_base_of_v<O, T>);
			return Pointer<O>(Release());
		}

		T* Release() { T* t = _t; _t = nullptr; return t; }

		~Pointer() { Free(); }

	 private:
		T* _t = nullptr;

		template<class I> friend class Pointer;
	};

	template<typename T, typename... ARGS> requires requires(ARGS&&... a) { T(std::forward<ARGS>(a)...); }
	ALWAYS_INLINE Pointer<T> ptr_create(ARGS&&... args)
	{
		return Pointer<T>(new T(std::forward<ARGS>(args)...));
	}

	/// Gives ownership of the pointer to the SmartPointer.
	template<typename T>
	ALWAYS_INLINE Pointer<T> ptr_cast(T* t)
	{
		return Pointer<T>(t);
	}
}
