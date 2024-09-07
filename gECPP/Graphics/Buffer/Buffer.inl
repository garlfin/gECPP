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
		_array = ReadArray<u32, T>(in);
	}

	template<class T>
	void Buffer<T>::IDeserialize(std::ostream& out) const
	{
		WriteArray<u32>(out, _array);
	}
}
