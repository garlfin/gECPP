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

	RelativePointer(T& t) : _t(&t) {};
	RelativePointer(T* t) : _t(t) {};

	OPERATOR_COPY_NOSUPER(RelativePointer,
		_t = (T*) ((u8*) o._t - (u8*) &o + (u8*) this);
		if(!o._t) _t = nullptr;
	);

	OPERATOR_MOVE_NOSUPER(RelativePointer,
		_t = (T*) ((u8*) o._t - (u8*) &o + (u8*) this);
		if(!o._t) _t = nullptr;
	);

	GET_SET(T*, , _t);

	ALWAYS_INLINE RelativePointer& operator=(T& t) { _t = &t; return *this; }
	ALWAYS_INLINE RelativePointer& operator=(T* t) { _t = t; return *this; }

	ALWAYS_INLINE T* operator->() const { return _t; }
	ALWAYS_INLINE T& operator*() const { return *_t; }
	ALWAYS_INLINE operator T&() const { return *_t; }
	ALWAYS_INLINE operator T*() const { return _t; }

private:
	T* _t = nullptr;
};
