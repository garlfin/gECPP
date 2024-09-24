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
	explicit RelativePointer(T* t) : _t(t) {};

	OPERATOR_COPY(RelativePointer, o,
		if(o._t)
			_t = (T*) ((u8*) o._t - (u8*) &o + (u8*) this);
		else
			_t = nullptr;
	);

	ALWAYS_INLINE RelativePointer& operator=(T& t) { _t = &t; return *this; }

	ALWAYS_INLINE T* operator->() const { return _t; }
	ALWAYS_INLINE T& operator*() const { return *_t; }
	explicit ALWAYS_INLINE operator T&() const { return *_t; }
	explicit ALWAYS_INLINE operator T*() const { return _t; }

private:
	T* _t = nullptr;
};
