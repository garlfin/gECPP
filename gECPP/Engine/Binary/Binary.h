//
// Created by scion on 8/16/2023.
//

#pragma once

#include "GL/Math.h"
#include <cstring>
#include <algorithm>
#include <vector>
#include "Engine/Array.h"
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

// Binary helper functions
u8* ReadFile(const char* name, u32& length, bool binary = false);
inline u8* ReadFile(const char* name, bool binary = false);

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

inline u8* ReadFile(const char* name, bool binary)
{
	u32 len;
	return ReadFile(name, len, binary);
}