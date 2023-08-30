//
// Created by scion on 8/29/2023.
//

#include "Binary.h"

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

	fseek(file, 0, SEEK_END);
	length = ftell(file);

	u8* bin = new u8[length + binary];
	if(binary) bin[length] = 0;

	fseek(file, 0, SEEK_SET);
	fread(bin, length, 1, file);

	return bin;
}