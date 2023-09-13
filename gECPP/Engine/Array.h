//
// Created by scion on 9/6/2023.
//

#pragma once

#include "GL/Math.h"
#include "GL/Binary.h"

template<typename T>
class Array
{
 public:
	explicit Array(u64 count, T* initialData = nullptr, u64 initialSize = 0);
	Array(const Array& o);
	Array(Array&& o) noexcept;
	Array();

	Array& operator =(const Array& o);
	Array& operator =(const Array&& o) noexcept;

	NODISCARD ALWAYS_INLINE u64 Size() const { return _size; }
	NODISCARD ALWAYS_INLINE T* Data() const { return _t; }
	NODISCARD ALWAYS_INLINE T& operator[](u64 i) { return _t[i]; }
	NODISCARD ALWAYS_INLINE const T& operator[](u64 i) const { return _t[i]; }

	~Array() { delete[] _t; }

 private:
	u64 _size;
	T* _t;
};

template<typename T>
Array<T>::Array() : _size(0), _t(nullptr)
{

}

template<typename T>
Array<T>& Array<T>::operator=(const Array&& o) noexcept
{
	_t = o._t;
	_size = o._size;

	o._t = nullptr;
	return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(const Array& o)
{
	this->~Array<T>();
	new(this) Array<T>(o);
	return *this;
}

template<typename T>
Array<T>::Array(Array&& o) noexcept : _size(o.Size()), _t(o._t)
{
	o._t = nullptr;
}

template<typename T>
Array<T>::Array(const Array& o) : _size(o.Size()), _t(new T[_size])
{
	memcpy(_t, o.Data(), o.Size() * sizeof(T));
}

template<typename T>
Array<T>::Array(u64 count, T* initialData, u64 initialSize) : _size(count), _t(new T[_size])
{
	memcpy(_t, initialData, initialData * sizeof(T));
}