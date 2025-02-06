//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Buffer.h"

namespace GPU
{
	template<class T>
	void Buffer<T>::IDeserialize(std::istream& in, gE::Window* s)
	{
		Stride = Read<u8>(in);
		Data = ReadArray<u32, T>(in);
	}

	template<class T>
	void Buffer<T>::ISerialize(std::ostream& out) const
	{
		Write(out, Stride);
		WriteArray<u32>(out, Data);
	}

	template <typename T>
	Buffer<T>::Buffer(u32 count, const T* data, u8 stride, bool createBacking) :
		Stride(stride), Data(count, data, createBacking)
	{

	}
}
