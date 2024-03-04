//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>
#include <cstring>
#include <algorithm>
#include <vector>
#include <Engine/Array.h>
#include "Macro.h"

using std::istream;
using std::ostream;

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
template<class T> void RemoveFirstFromVec(std::vector<T>& vec, const T& t);
template<typename UINT_T, class T> Array<T> ReadArray(istream&);

// Binary helper functions
std::string ReadPrefixedString(istream&);
void WritePrefixedString(ostream&, const std::string&);

u8* ReadFile(const char* name, u32& length, bool binary = false);
inline u8* ReadFile(const char* name, bool binary = false);

template<typename T> T Read(istream& src);
template<typename T> void Read(istream& src, T* ts, u32 count);
template<u64 COUNT, typename T> void Read(istream& src, T* ts);

template<typename T> void Write(ostream& src, const T&);
template<typename T> void Write(ostream& src, T* ts, u32 count);
template<u32 COUNT, typename T> void Write(ostream& src, T* ts);

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
T Read(istream& src)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	T t;
	src.read((char*) &t, sizeof(T));
	return t;
}

template<typename T>
void Read(istream& src, T* ts, u32 count)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	if(!count) return;
	src.read((char*) ts, sizeof(T) * count);
}

template<u64 COUNT, typename T>
void Read(istream& src, T* ts)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");
	static_assert(COUNT > 0);

	src.read((char*) ts, sizeof(T) * COUNT);
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

template<typename UINT_T, class T>
Array<T> ReadArray(istream& src)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	UINT_T count = ::Read<UINT_T>(src);
	Array<T> arr(count);

	src.read(arr.Data(), sizeof(T) * count);
	return arr;
}

template<typename T> void Write(ostream& src, const T& t)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	src.write((char*) &t, sizeof(T));
}

template<typename T> void Write(ostream& src, T* ts, u32 count)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	if(!count) return;
	src.write(ts, sizeof(T) * count);
}

template<u32 COUNT, typename T> void Write(ostream& src, T* ts)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");
	static_assert(COUNT > 0);

	src.write(ts, sizeof(T) * COUNT);
}