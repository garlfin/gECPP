//
// Created by scion on 9/6/2023.
//

#pragma once

#include "Engine/Math/Math.h"
#include <Engine/Binary/Macro.h>

template<typename T>
class Array
{
 public:
	static constexpr bool IS_VOID = std::is_same_v<T, void>;
	using I = std::conditional_t<IS_VOID, u8, T>;
	
	Array() = default;

	template<typename... ARGS>
	explicit Array(size_t count, ARGS&&... args) : _size(count), _t(new I[count])
	{
		if constexpr (sizeof...(ARGS) && !IS_VOID)
			for(int i = 0; i < count; i++) _t[i] = I(std::forward<ARGS>(args)...);
	}

	Array(size_t count, I* t) : _size(count), _t(new I[count])
	{
		if(t) memcpy(_t, t, count * sizeof(I));
	}

	OPERATOR_EQUALS(Array, o,
	{
		LOG("WARNING: REALLOCATION! \n\tSIZE: " << o._size * sizeof(I) << " bytes\n\tFUNCTION: " << __PRETTY_FUNCTION__);

		_size = o._size;
		_t = new I[_size];

		for (int i = 0; i < _size; i++) _t[i] = o._t[i];
	})

	OPERATOR_EQUALS_XVAL(Array, o,
	{
		_size = o._size;
		_t = o._t;
		o._t = nullptr;
	})

	size_t CopyToCArray(I* arr, size_t arrSize) const;

	template<size_t COUNT>
	ALWAYS_INLINE size_t CopyToCArray(I(& arr)[COUNT]) const { return CopyToCArray(arr, COUNT); }

	NODISCARD ALWAYS_INLINE u64 Count() const { return _size; }
	NODISCARD ALWAYS_INLINE I* Data() { return _t; }
	NODISCARD ALWAYS_INLINE const I* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE I& operator[](u64 i) { return _t[i]; }
	NODISCARD ALWAYS_INLINE const I& operator[](u64 i) const { return _t[i]; }

	NODISCARD ALWAYS_INLINE bool IsFree() const { return _t; }
	ALWAYS_INLINE void Free() { delete[] _t; _t = nullptr; _size = 0; }

	ALWAYS_INLINE operator bool() const { return IsFree(); }

	~Array() { Free(); }

 private:
	u64 _size = 0;
	I* _t = nullptr;
};

template<typename T>
size_t Array<T>::CopyToCArray(I* arr, size_t arrSize) const
{
	size_t copyCount = std::min(arrSize, _size);

	if constexpr(std::is_trivially_copyable_v<T>)
		memcpy(arr, _t, copyCount * sizeof(T));
	else
		for(size_t i = 0; i < copyCount; i++)
			arr[i] = _t[i];

	return copyCount;
}