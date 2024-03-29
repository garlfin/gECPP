//
// Created by scion on 9/6/2023.
//

#pragma once

#include "GL/Math.h"
#include <GL/Binary/Macro.h>

template<typename T>
class Array
{
 public:
	explicit Array(u64 count);
	Array() = default;

	Array(const Array& o) : _size(o.Count()), _t(new T[_size]{}) { memcpy(_t, o.Data(), o.Count() * sizeof(T)); };
	Array(Array&& o) noexcept : _size(o.Count()), _t(o._t) { o._t = nullptr; };

	OPERATOR_EQUALS_BOTH(Array<T>);

	size_t CopyToCArray(T* arr, size_t arrSize) const;
	template<size_t COUNT>
	ALWAYS_INLINE size_t CopyToCArray(T(& arr)[COUNT]) const { return CopyToCArray(arr, COUNT); }

	NODISCARD ALWAYS_INLINE u64 Count() const { return _size; }
	NODISCARD ALWAYS_INLINE T* Data() { return _t; }
	NODISCARD ALWAYS_INLINE const T* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE T& operator[](u64 i) { return _t[i]; }
	NODISCARD ALWAYS_INLINE const T& operator[](u64 i) const { return _t[i]; }

	~Array() { delete[] _t; _t = nullptr; }

 private:
	u64 _size = 0;
	T* _t = nullptr;
};

template<typename T>
size_t Array<T>::CopyToCArray(T* arr, size_t arrSize) const
{
	size_t copyCount = std::min(arrSize, _size);

	if constexpr(std::is_trivially_copyable_v<T>)
		memcpy(arr, _t, copyCount * sizeof(T));
	else
		for(size_t i = 0; i < copyCount; i++)
			arr[i] = _t[i];

	return copyCount;
}

template<typename T>
Array<T>::Array(u64 count) : _size(count), _t(new T[_size] {})
{
	for(u64 i = 0; i < count; i++) _t[i] = T();
}