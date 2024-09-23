//
// Created by scion on 9/23/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include "Macro.h"

template<class T>
struct RelativePointer
{
public:
	RelativePointer() = default;
	explicit RelativePointer(T* t) : _t(t) {};

	OPERATOR_COPY(RelativePointer, o, _t = (T*) ((u8*) o._t - (u8*) &o + (u8*) this));

	ALWAYS_INLINE T* operator->() { return _t; }
	ALWAYS_INLINE const T* operator->() const { return _t; }

	ALWAYS_INLINE T& operator*() { return *_t; }
	ALWAYS_INLINE const T& operator*() const { return *_t; }

	ALWAYS_INLINE operator T*() { return _t; }
	ALWAYS_INLINE operator const T*() const { return _t; }
private:
	T* _t;
};
