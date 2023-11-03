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
	Nullable(const T& t) : _exists(true), _t(t) {};
	Nullable() {};

	GET(T&, Value, _t);

	inline void SetValue(T&& t);
	inline void SetValue(T* t);
	inline void SetEmpty();

	ALWAYS_INLINE operator bool() const { return _exists; }
	ALWAYS_INLINE T* operator->() const { return _t; }

	~Nullable() { if(_exists) _t.~T(); }

 private:
	bool _exists = false;
	union
	{
		u8 _tBuf[sizeof(T)] {};
		T _t;
	};
};

template<class T>
inline void Nullable<T>::SetValue(T&& t)
{
	_t = std::move(t);
	_exists = true;
};

template<class T>
inline void Nullable<T>::SetValue(T* t)
{
	_exists = t;
	if(t) _t = *t;
}

template<class T>
inline void Nullable<T>::SetEmpty()
{
	if(!_exists) return;

	_exists = false;
	_t.~T();
	memset(_tBuf, sizeof(T), 0);
}

