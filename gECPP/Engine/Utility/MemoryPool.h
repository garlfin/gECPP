//
// Created by scion on 8/25/2023.
//

#pragma once
#include <cassert>
#include <iostream>

#include "Macro.h"

template<class T, u64 PAGE_T_SIZE = 64>
struct MemoryPool
{
 public:
	MemoryPool() : _page((T*) new u8[PAGE_T_SIZE * (sizeof(T) + 1)]{}), _pageFlags((u8*) &_page[PAGE_T_SIZE]) { }

	template<typename... ARGS>
	T* New(ARGS&& ... args)
	{
		for(u64 i = 0; i < PAGE_T_SIZE; i++)
			if(!_pageFlags[i])
			{
				_pageFlags[i] = 1;
				return new(&_page[i]) T(args...);
			}

		GE_FAIL("Failed to place T in Memory Pool!");
	}

	void Delete(T* t)
	{
		u32 i = u64(t - _page) / sizeof(T);
		GE_ASSERTM(t == &_page[i], "Pointer is not aligned!");

		if(!_pageFlags[i]) return;

		_pageFlags[i] = 0;
		if constexpr(std::is_destructible_v<T>) _page[i].~T();
	}

	NODISCARD ALWAYS_INLINE T* Data() const { return _page; }

	NODISCARD u32 Count() const
	{
		u32 result = 0;
		for(u32 i = 0; i < PAGE_T_SIZE; i++)
			if(_pageFlags[i]) result++;

		return result;
	}

	~MemoryPool() { ::operator delete[](_page); }

 private:
	T* _page;
	u8* _pageFlags;
};
