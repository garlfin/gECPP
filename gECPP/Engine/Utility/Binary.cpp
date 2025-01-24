//
// Created by scion on 8/29/2023.
//

#include "Binary.h"

#include <fstream>

u8* ReadFileBinary(const Path& path, size_t& length)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		LOG("Could not find file: " << path);
		return nullptr;
	}

	file.seekg(0, std::ios::end);
	length = file.tellg();

	u8* bin = new u8[length];

	file.seekg(0, std::ios::beg);
	file.read((char*) bin, length);

	return bin;
}

std::string ReadFile(const Path& path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		LOG("Could not find file: " << path);
		return DEFAULT;
	}

	file.seekg(0, std::ios::end);
	size_t length = file.tellg();

	std::string result;
	result.reserve(length);

	file.seekg(0, std::ios::beg);
	file.read(result.data(), length);

	return result;
}

size_t strlenc(const char* str, char d)
{
	u64 i = 0;
	for(; str[i] && str[i] != d; i++) {}
	return i;
}

size_t strlencLast(const char* str, char c, char d)
{
	size_t len = strlenc(str, d);
	size_t i = 0;

	while(i < len)
	{
		i++; // i starts off as the end character
		if(*(str + len - i) == c) return len - i;
	}

	return -1;
}

char* strdupc(const char* str, char d)
{
	size_t len = strlenc(str, d);
	if(!len) return nullptr;
	char* newStr = new char[len + 1]{};
	return (char*) memcpy(newStr, str, len);
}

const char* IncrementLine(const char* str, char d)
{
	for(; *str != d; str++) if(!*str) return nullptr;
	return ++str;
}

bool strcmpb(const char* a, const char* b, u32 length)
{
	for(u32 i = 0; i < length; i++)
		if(a[i] != b[i]) return false;
	return true;
}
