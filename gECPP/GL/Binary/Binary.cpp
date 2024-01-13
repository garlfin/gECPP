//
// Created by scion on 8/29/2023.
//

#include "Binary.h"
#include "iostream"
#include "SerializationBuffer.h"

using namespace gETF;

char* ReadPrefixedString(u8*& ptr)
{
	u8 len = Read<u8>(ptr);
	if(!len) return nullptr;
	char* str = new char[len + 1];
	Read<char>(ptr, str, len);
	str[len] = 0;
	return str;
}

u8* ReadFile(const char* name, u32& length, bool binary)
{
	FILE* file = fopen(name, "rb");
	if(!file)
	{
		LOG("Could not find file: " << name);
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	length = ftell(file);

	u8* bin = new u8[length + !binary];
	if(!binary) bin[length] = 0;

	fseek(file, 0, SEEK_SET);
	fread(bin, length, 1, file);

	fclose(file);

	return bin;
}

size_t strlenc(const char* str, char d)
{
	u64 i = 0;
	for(; str[i] && str[i] != d; i++);
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

template<>
void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count)
{
	for(u32 i = 0; i < count; i++) PushPtr(t[i].Data(), t[i].Length());
}

void SerializationBuffer::Realloc(u64 newSize)
{
	u64 logSize = 1 << (BIT_SIZE(newSize) - __builtin_clzll(newSize)); // count leading zeros
	if(_alloc != logSize) _buf = (u8*) realloc(_buf, logSize);
	_alloc = logSize;
	_size = newSize;
}

void SerializationBuffer::PushLengthString(const char* ptr)
{
	u8 len = ptr ? MIN(strlen(ptr), UINT8_MAX) : 0;
	Push(len);
	PushPtr(ptr, len);
}

void SerializationBuffer::StrCat(const char* str, bool incTerminator, char d)
{
	GE_ASSERT(str, "'str' should have a value.");

	u32 strLen = strlenc(str, d);
	if(str[strLen] == d) strLen += incTerminator;
	u64 preSize = Length();

	Realloc(_size + strLen);
	memcpy(_buf + preSize, str, strLen);
}

void SerializationBuffer::FromFile(const char* file, bool binary)
{
	u64 preSize = _size, length;

	FILE* fstream = fopen(file, "rb");

	fseek(fstream, 0, SEEK_END);
	length = ftell(fstream);

	Realloc(_size + length + !binary);

	if(!binary) _buf[_size - 1] = 0;

	fseek(fstream, 0, SEEK_SET);
	fread(_buf + preSize, length, 1, fstream);
}

char* SerializationBuffer::Find(const char* str, char delimiter)
{
	char* s = (char*) _buf;
	char* end = (char*) _buf + _size;
	size_t len = strlenc(str, delimiter);

	while(s < end)
	{
		if(!memcmp(s, str, std::min(end - s, (long long) len)))
			return s;
		s++;
	}

	return nullptr;
}
void SerializationBuffer::ToFile(const char* file)
{
	FILE* fstream = fopen(file, "wb");
	fwrite(_buf, 1, _size, fstream);
	fclose(fstream);
}

