//
// Created by scion on 9/6/2023.
//

#pragma once

#include <Core/Log.h>
#include <Core/Macro.h>
#include <Core/Math/Math.h>

template<class T>
using Range = std::ranges::subrange<T>;

template<typename T> requires (!std::is_same_v<T, void>)
class Array
{
public:
	constexpr Array() = default;

	using iterator = T*;

	Array(const std::initializer_list<T>& init) : _size(init.size())
	{
		if(_size) _t = new T[_size];
		for(size_t i = 0; i < _size; i++)
			_t[i] = *(init.begin() + i);
	}

	template<typename... ARGS>
	explicit Array(size_t count, ARGS&&... args) : _size(count), _t(nullptr)
	{
		if(_size) _t = new T[count];
		if constexpr (sizeof...(ARGS))
			for(size_t i = 0; i < count; i++) _t[i] = I(std::forward<ARGS>(args)...);
	}

	Array(size_t count, const T* t, bool createBacking = true) : _size(count), _t(nullptr)
	{
		if(t) GE_ASSERT(createBacking);
		if(_size && createBacking) _t = new T[count];

		if(!t || !createBacking) return;
		if constexpr(std::is_trivially_copyable_v<T>)
			memcpy(_t, t, count * sizeof(T));
		else
			for(size_t i = 0; i < count; i++)
				_t[i] = t[i];
	}

	OPERATOR_COPY_NOSUPER(Array, Free,
	{
		_size = o._size;

		if(!_size || !o._t) return;

		gE::Log::Write("WARNING: REALLOCATION! \n\tSIZE: {} bytes\n\tFUNCTION: {}\n", o._size * sizeof(T), PRETTY_FUNCTION);

		_t = new T[_size];
		if constexpr(std::is_trivially_copyable_v<T>)
			memcpy(_t, o._t, _size * sizeof(T));
		else
			for(size_t i = 0; i < _size; i++)
				_t[i] = o._t[i];
	});

	OPERATOR_MOVE_NOSUPER(Array, Free,
	{
		_size = o._size;
		_t = o._t;
		o._t = nullptr;
	});

	size_t CopyToCArray(T* arr, size_t arrSize) const
	{
		size_t copyCount = std::min(arrSize, _size);

		if constexpr(std::is_trivially_copyable_v<T>)
			memcpy(arr, _t, copyCount * sizeof(T));
		else
			for(size_t i = 0; i < copyCount; i++)
				arr[i] = _t[i];

		return copyCount;
	}

	template<size_t COUNT>
	ALWAYS_INLINE size_t CopyToCArray(T(& arr)[COUNT]) const { return CopyToCArray(arr, COUNT); }

	NODISCARD ALWAYS_INLINE u64 Size() const { return _size; }
	NODISCARD ALWAYS_INLINE u64 ByteSize() const { return _size * sizeof(T); }
	NODISCARD ALWAYS_INLINE T* Data() { return _t; }
	NODISCARD ALWAYS_INLINE const T* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE T& operator[](u64 i) const { GE_ASSERTM(_t, "ERROR: ARRAY NOT INITIALIZED"); return _t[i]; }

	NODISCARD ALWAYS_INLINE bool IsFree() const { return !_t; }
	ALWAYS_INLINE void Free() { delete[] _t; _t = nullptr; }

	ALWAYS_INLINE operator bool() const { return _t; }
	ALWAYS_INLINE T* operator*() const { return _t; }

	T* begin() const { return _t; }
	T* end() const { return _t + _size; }

	~Array() { Free(); }

private:
	u64 _size = 0;
	T* _t = nullptr;
};

template<class T, size_t SIZE> requires (!std::is_same_v<T, void>)
class SmallVector
{
public:
	using iterator = T*;
	using const_iterator = const T*;

	constexpr SmallVector() = default;

	constexpr SmallVector(const std::initializer_list<T>& init) : _size(init.size())
	{
		for(size_t i = 0; i < _size; i++)
			_t[i] = *(init.begin() + i);
	}

	template<typename... ARGS>
	explicit SmallVector(size_t count, ARGS&&... args) : _size(count), _t(nullptr)
	{
		if constexpr (sizeof...(ARGS))
			for(size_t i = 0; i < count; i++) _t[i] = I(std::forward<ARGS>(args)...);
	}

	OPERATOR_COPY_IMPL(, SmallVector,,
	{
		_size = o._size;

		if(!_size || !o._t) return;

		if constexpr(std::is_trivially_copyable_v<T>)
			memcpy(_t, o._t, _size * sizeof(T));
		else
			for(size_t i = 0; i < _size; i++)
				_t[i] = o._t[i];
	});

	constexpr bool PushBack(const T& t)
	{
		if(!HasCapacity())
			return false;
		_t[_size++] = t;
		return true;
	}

	template<typename... ARGS>
	constexpr bool EmplaceBack(ARGS&&... args)
	{
		if(!HasCapacity())
			return false;
		_t[_size++] = T(std::forward<ARGS>(args)...);
		return true;
	}

	constexpr bool Insert(iterator it, const T& t)
	{
		if(it == end())
			return PushBack(t);

		if(HasCapacity()) ++_size;
		for(iterator swapIt = it; swapIt != back(); ++swapIt)
			*(swapIt + 1) = *swapIt;
		*it = t;

		return true;
	}

	NODISCARD ALWAYS_INLINE constexpr bool HasCapacity() const { return _size < SIZE; }
	NODISCARD ALWAYS_INLINE constexpr u64 Capacity() const { return SIZE; }
	NODISCARD ALWAYS_INLINE constexpr u64 Size() const { return _size; }
	NODISCARD ALWAYS_INLINE constexpr u64 ByteSize() const { return _size * sizeof(T); }
	NODISCARD ALWAYS_INLINE constexpr T* Data() { return _t; }
	NODISCARD ALWAYS_INLINE constexpr const T* Data() const { return _t; }

	NODISCARD ALWAYS_INLINE constexpr T& operator[](u64 i) { return _t[i]; }
	NODISCARD ALWAYS_INLINE constexpr const T& operator[](u64 i) const { return _t[i]; }

	ALWAYS_INLINE constexpr operator bool() const { return _size; }
	ALWAYS_INLINE constexpr T* operator*() const { return _t; }

	constexpr T* begin() { return _t; }
	constexpr T* end() { return _t + _size; }
	constexpr const T* begin() const { return _t; }
	constexpr const T* end() const { return _t + _size; }

	constexpr T* back() { return _t + _size - 1; }
	constexpr const T* back() const { return _t + _size - 1; }

private:
	size_t _size = 0;
	T _t[SIZE] {};
};

template<class FIRST_ARR_T, class SECOND_ARR_T>
class PairSpan
{
public:
	using FIRST_IT_T = typename FIRST_ARR_T::iterator;
	using SECOND_IT_T = typename SECOND_ARR_T::iterator;

	struct iterator
	{
		FIRST_IT_T first;
		SECOND_IT_T second;

		NODISCARD bool operator==(const iterator& b) { return first == b.first || second == b.second; }
		NODISCARD iterator& operator++() { first++; second++; return *this; };
	};

	PairSpan(const FIRST_ARR_T& first, const FIRST_ARR_T& second) :
		Begin(first.begin(), second.begin()),
		End(first.end(), second.end())
	{};

	iterator Begin = DEFAULT;
	iterator End = DEFAULT;

	iterator begin() const { return Begin; }
	iterator end() const { return End; }
};