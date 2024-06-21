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

#include <ostream>
#include <istream>

#define DIV_CEIL(X, Y) (((X) + decltype(X)(Y) - decltype(X)(1)) / decltype(X)(Y))
#define DIV_CEIL_T(X, Y, T) (((T)(X) + (T)(Y) - (T)(1)) / (T)(Y))

using std::istream;
using std::ostream;

// CSTDLIB alternatives
size_t strlenc(const char*, char);
size_t strlencLast(const char*, char, char = 0);
char* strdupc(const char*, char);
bool strcmpb(const char* a, const char* b, u32 length);

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

template<typename T> void Read(istream& src, T*, u64);
template<typename T> inline void Read(istream& src, T& t) { Read<T>(src, &t, 1); }
template<typename T> inline T Read(istream& src) { T t; Read<T>(src, t); return t; }

template<typename T> void Write(ostream& src, const T* ts, u64 count);
template<typename T> inline void Write(ostream& src, const T& t) { Write<T>(src, &t, 1); }

// Implementation

template<u32 LENGTH>
bool strcmpb(const char* a, const char(& b)[LENGTH])
{
	for(u32 i = 0; i < LENGTH - 1; i++) // exclude null terminator
		if(a[i] != b[i]) return false;
	return true;
}

template<typename T>
void Read(istream& src, T* ts, u64 count)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	if(!count) return;
	src.read((char*) ts, sizeof(T) * count);
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

template<typename T> void Write(ostream& src, const T* ts, u64 count)
{
	static_assert(std::is_trivially_copyable_v<T>, "T MUST BE TRIVIALLY COPYABLE");

	if(!count) return;
	src.write((char*) ts, sizeof(T) * count);
}