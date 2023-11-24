//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>
#include <cstring>
#include <algorithm>

#include "Macro.h"
#include "SerializationBuffer.h"

// CSTDLIB alternatives
size_t strlenc(const char*, char);
size_t strlencLast(const char*, char, char = 0);
char* strdupc(const char*, char);

template<u32 LENGTH>
bool strcmpb(const char* a, const char* b);
template<u32 LENGTH>
bool strcmpb(const char* a, const char(& b)[LENGTH]);

// Various helper functions
const char* IncrementLine(const char* s, char d = '\n');
template<class T>
void RemoveFirstFromVec(std::vector<T>& vec, const T& t);

// Binary helper functions
char* ReadPrefixedString(u8*& ptr);
u8* ReadFile(const char* name, u32& length, bool binary = false);
inline u8* ReadFile(const char* name, bool binary = false);

template<typename T>
T Read(u8*& src);
template<typename T>
void Read(u8*& src, T* ts, u32 count);
template<typename T, u32 COUNT>
void Read(u8*& src, T* ts);

// Implementation
template<u32 LENGTH>
bool strcmpb(const char* a, const char* b)
{
	for(u32 i = 0; i < LENGTH; i++)
		if(a[i] != b[i]) return false;
	return true;
}

template<u32 LENGTH>
bool strcmpb(const char* a, const char(& b)[LENGTH])
{
	for(u32 i = 0; i < LENGTH - 1; i++) // exclude null terminator
		if(a[i] != b[i]) return false;
	return true;
}

template<typename T>
T Read(u8*& src)
{
	static_assert(!std::is_base_of_v<gETF::Serializable, T>, "DONT USE THIS W/ SERIALIZABLES!");
	T t = *(T*) src;
	src += sizeof(T);
	return t;
}

template<typename T>
void Read(u8*& src, T* ts, u32 count)
{
	if constexpr(std::is_base_of_v<gETF::Serializable, T>)
	{
		for(u32 i = 0; i < count; i++)
			ts[i].Serialize(src);
	}
	else
	{
		memcpy(ts, src, count * sizeof(T));
		src += count * sizeof(T);
	}
}

template<typename T, u32 COUNT>
void Read(u8*& src, T* ts)
{
	memcpy(ts, src, COUNT * sizeof(T));
	src += COUNT * sizeof(T);
}

inline u8* ReadFile(const char* name, bool binary)
{
	u32 len;
	return ReadFile(name, len, binary);
}

template<class T>
void RemoveFirstFromVec(std::vector<T>& vec, const T& t)
{
	auto f = std::find(vec.begin(), vec.end(), t);
	if(f == vec.end()) return;
	std::iter_swap(f, vec.end() - 1);
	vec.pop_back();
}