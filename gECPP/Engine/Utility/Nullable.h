//
// Created by scion on 11/3/2023.
//

#pragma once

#include "Macro.h"

template<class T>
class Nullable
{
 public:
	inline explicit Nullable(T* t) : _t(t) { };
	Nullable() = default;

	GET(T&, Value, _t);
	SET(T*,, _t);

	ALWAYS_INLINE explicit operator T&() const { return *_t; }
	ALWAYS_INLINE explicit operator T*() const { return _t; }
	ALWAYS_INLINE operator bool() const { return _t; } // NOLINT

	ALWAYS_INLINE const T* operator->() const
	{
		GE_ASSERTM(_t, "NULL USAGE!");
		return _t;
	}

	ALWAYS_INLINE T* operator->()
	{
		GE_ASSERTM(_t, "NULL USAGE!");
		return _t;
	}

	ALWAYS_INLINE const T& operator||(const T& t) const { return _t ? *_t : t; };
	ALWAYS_INLINE T& operator||(T& t) const { return _t ? *_t : t; }
	ALWAYS_INLINE const T* operator||(const T* t) const { return _t ?: t; };
	ALWAYS_INLINE T* operator||(T* t) const { return _t ?: t; }

 private:
	T* _t = nullptr;
};