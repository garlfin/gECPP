//
// Created by scion on 8/16/2023.
//

#pragma once

#include <algorithm>
#include <cstring>
#include <istream>
#include <ostream>
#include <vector>

#include "Macro.h"
#include "Engine/Math/Math.h"

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
const char* IncrementLine(const char* str, char d = '\n');
template<class T> void RemoveFirstFromVec(std::vector<T>& vec, const T& t);

// Binary helper functions
u8* ReadFileBinary(const Path& path, size_t& length);
std::string ReadFile(const Path& path);
inline u8* ReadFileBinary(const Path& name);

// Implementation
template<u32 LENGTH>
bool strcmpb(const char* a, const char(& b)[LENGTH])
{
	for(u32 i = 0; i < LENGTH - 1; i++) // exclude null terminator
		if(a[i] != b[i]) return false;
	return true;
}

template<class T>
void RemoveFirstFromVec(std::vector<T>& vec, const T& t)
{
	auto f = std::find(vec.begin(), vec.end(), t);
	if(f == vec.end()) return;
	std::iter_swap(f, vec.end() - 1);
	vec.pop_back();
}

ALWAYS_INLINE u8* ReadFileBinary(const Path& name)
{
	size_t len;
	return ReadFileBinary(name, len);
}