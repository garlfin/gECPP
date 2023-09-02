//
// Created by scion on 8/29/2023.
//

#include "Binary.h"
#include "GL.h"
#include "iostream"

using namespace gETF;

char* ReadString(u8*& ptr)
{
	u8 len = Read<u8>(ptr);
	char* str = new char[len + 1];
	Read<char>(ptr, str, len);
	str[len] = 0;
	return str;
}

u8* ReadFile(const char* name, u32& length, bool binary)
{
	FILE* file = fopen(name, "rb");
	if(!file) return nullptr;

	fseek(file, 0, SEEK_END);
	length = ftell(file);

	u8* bin = new u8[length + !binary];
	if(!binary) bin[length] = 0;

	fseek(file, 0, SEEK_SET);
	fread(bin, length, 1, file);

	return bin;
}

size_t strlenc(const char* str, char d)
{
	const char* s;
	for(s = str; *s != d; s++) if(!*s) return s - str; // no clue why i need this, i was playing around...
	return s - str;
}

const char* IncrementLine(const char* str, char d)
{
	for(; *str != d; str++) if(!*str) return nullptr;
	return ++str;
}

template<>
void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count)
{
	for (u32 i = 0; i < count; i++) PushPtr(t[i].Data(), t[i].Length());
}

void SerializationBuffer::Realloc(u64 newSize)
{
	u64 logSize = 1 << (BIT_SIZE(newSize) - __builtin_clzll(newSize));
	if(_alloc != logSize) _buf = (u8*) realloc(_buf, logSize);
	_alloc = logSize;
	_size = newSize;
}

void SerializationBuffer::PushString(const char* ptr)
{
	u8 len = MIN(strlen(ptr), UINT8_MAX);
	Push(len);
	PushPtr(ptr, len);
}

void SerializationBuffer::StrCat(const char* str, char d)
{
	if(!str) return;
	u32 strLen = strlenc(str, d);
	u64 last = MAX(_size, 1);

	Realloc(last + strLen);
	memcpy((char*) _buf + last - 1, str, strLen);
	_buf[_size - 1] = 0;
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

void SerializationBuffer::SafeMemCpy(const u8* ptr, u64 len, u64 offset)
{
	if(_buf + offset + len >= _buf + _alloc)
		GE_FAIL("WROTE OUT OF BOUNDS");
	memcpy(_buf + offset, ptr, len);
}
