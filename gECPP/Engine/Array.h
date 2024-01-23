//
// Created by scion on 9/6/2023.
//

#pragma once

#include "GL/Math.h"
#include "GL/Binary/Binary.h"

template<typename T>
class Array
{
 public:
	explicit Array(u64 count, T* initialData = nullptr, u64 initialSize = 0);
	Array() = default;

	Array(const Array& o) : _size(o.Count()), _t(new T[_size]{}) { memcpy(_t, o.Data(), o.Count() * sizeof(T)); };
	Array(Array&& o) noexcept : _size(o.Count()), _t(o._t) { o._t = nullptr; };

	OPERATOR_EQUALS_BOTH(Array<T>);

	template<size_t COUNT, typename I>
	size_t CopyToCArray(I(& arr)[COUNT]) const;
	size_t CopyToCArray(T* arr, size_t arrSize) const;

	NODISCARD ALWAYS_INLINE u64 Count() const { return _size; }
	NODISCARD ALWAYS_INLINE T* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE T& operator[](u64 i) { return _t[i]; }
	NODISCARD ALWAYS_INLINE const T& operator[](u64 i) const { return _t[i]; }

	~Array() { delete[] _t; }

 private:
	u64 _size = 0;
	T* _t = nullptr;
};

template<typename T>
size_t Array<T>::CopyToCArray(T* arr, size_t arrSize) const
{
	size_t copyCount = std::min(arrSize, _size);

	if constexpr(std::is_trivially_constructible_v<T>)
		memcpy(arr, _t, copyCount * sizeof(T));
	else
		for(size_t i = 0; i < copyCount; i++)
			arr[i] = _t[i];

	return copyCount;
}

template<typename T>
template<size_t COUNT, typename I>
size_t Array<T>::CopyToCArray(I (& arr)[COUNT]) const
{
	size_t copyCount = std::min(COUNT, _size);

	if constexpr(std::is_trivially_constructible_v<T>)
		memcpy(arr, _t, copyCount * sizeof(T));
	else
		for(size_t i = 0; i < copyCount; i++)
			arr[i] = _t[i];

	return copyCount;
}

template<typename T>
Array<T>::Array(u64 count, T* initialData, u64 initialSize) : _size(count), _t(new T[_size])
{
	if(initialData && initialSize) memcpy(_t, initialData, initialSize * sizeof(T));
}