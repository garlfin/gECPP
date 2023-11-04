//
// Created by scion on 11/3/2023.
//

#pragma once

#include <cstring>
#include <GL/Math.h>
#include "Macro.h"

template<class T>
class Nullable
{
 public:
	inline explicit Nullable(T* t) : _t(t)
	{ };

	inline explicit Nullable(const Nullable& o) : _t(o._t)
	{ };
	Nullable() = default;

	OPERATOR_EQUALS(Nullable);

	GET(T &, Value, _t);

	inline void Set(T* t = nullptr);

	ALWAYS_INLINE explicit operator T&() const
	{ return *_t; }

	ALWAYS_INLINE explicit operator T*() const
	{ return _t; }

	ALWAYS_INLINE operator bool() const
	{ return _t; } // NOLINT

	ALWAYS_INLINE const T* operator->() const
	{

		LOG("WARNING! NULL USAGE IN " << __PRETTY_FUNCTION__ << ".\n");
		return _t;
	}

	ALWAYS_INLINE T* operator->()
	{

		LOG("WARNING! NULL USAGE IN " << __PRETTY_FUNCTION__ << ".\n");
		return _t;
	}

	ALWAYS_INLINE const T& operator||(const T& t) const
	{ return _t ? *_t : t; };

	ALWAYS_INLINE T& operator||(T& t)
	{ return _t ? *_t : t; }

	ALWAYS_INLINE const T* operator||(const T* t) const
	{ return _t ?: t; };

	ALWAYS_INLINE T* operator||(T* t) const
	{ return _t ?: t; }

 private:
	T* _t = nullptr;
};

template<class T>
inline void Nullable<T>::Set(T* t)
{

	_t = t;
};
