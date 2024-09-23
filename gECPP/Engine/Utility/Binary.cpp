//
// Created by scion on 8/29/2023.
//

#include "Binary.h"
#include "iostream"

u8* ReadFileBinary(const std::string& name, u32& length)
{
	FILE* file = fopen(name.c_str(), "rb");
	if(!file)
	{
		LOG("Could not find file: " << name);
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);

	u8* bin = new u8[length];

	fseek(file, 0, SEEK_SET);
	fread(bin, length, 1, file);

	fclose(file);

	return bin;
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
