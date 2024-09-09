//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Buffer.h"

namespace GPU
{
	template<class T>
	void Buffer<T>::ISerialize(std::istream& in, gE::Window* s)
	{
		Stride = Read<u8>(in);
		Data = ReadArray<u32, T>(in);
	}

	template<class T>
	void Buffer<T>::IDeserialize(std::ostream& out) const
	{
		Write(out, Stride);
		WriteArray<u32>(out, Data);
	}
}
