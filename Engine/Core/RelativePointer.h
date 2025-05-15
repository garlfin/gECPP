//
// Created by scion on 9/23/2024.
//

#pragma once

#include <Core/Math/Math.h>
#include <Core/Serializable/Serializable.h>

#include "Macro.h"

template<class T>
struct RelativePointer
{
public:
	RelativePointer() = default;

	RelativePointer(T& t) : _t(&t) {};
	RelativePointer(T* t) : _t(t) {};

	OPERATOR_COPY_IMPL(, RelativePointer,,
		_t = (T*) ((u64) o._t - (u64) &o + (u64) this);
		if(!o._t) _t = nullptr;
	);

	OPERATOR_MOVE_IMPL(, RelativePointer,,
		_t = (T*) ((std::byte*) o._t - (std::byte*) &o + (std::byte*) this);
		if(!o._t) _t = nullptr;
	);

	GET_SET(T*, , _t);
	GET(T*, Pointer, _t);

	ALWAYS_INLINE RelativePointer& operator=(T& t) { _t = &t; return *this; }
	ALWAYS_INLINE RelativePointer& operator=(T* t) { _t = t; return *this; }

	ALWAYS_INLINE T* operator->() const { return _t; }
	ALWAYS_INLINE T& operator*() const { return *_t; }
	ALWAYS_INLINE operator T&() const { return *_t; }
	ALWAYS_INLINE operator T*() const { return _t; }

private:
	T* _t = nullptr;
};

template<class T>
void Read(std::istream& in, RelativePointer<T>& pointer)
{
	T* raw = (T*)((std::byte*) &pointer + Read<u64>(in));
	pointer = raw;
}

template<class T>
void Write(std::ostream& out, const RelativePointer<T>& pointer)
{
	Write(out, (u64) ((std::byte*) pointer.GetPointer() - (std::byte*) &pointer));
}
