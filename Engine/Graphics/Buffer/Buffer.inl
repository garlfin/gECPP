//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Buffer.h"
#include <Core/GUI/Field.h>

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

	template<class T>
	REFLECTABLE_ONGUI_IMPL(Buffer<T>,
	{
		ImGui::TextUnformatted(std::format("Array of {}:\n\tSize: {}\n\tByte Count: {}\n\tPointer: {}",
			demangle(typeid(T).name()),
			Data.Count(),
			Data.ByteCount(),
			(void*) Data.Data()
		).c_str());
	});

	template<class T>
	Buffer<T>* Buffer<T>::Factory(std::istream& in, SETTINGS_T t)
	{
		return new Buffer(in, t);
	};

	template<class T>
	void Buffer<T>::UFactory(std::istream& in, SETTINGS_T t, Buffer& result)
	{
		PlacementNew(result, in, t);
	};
}
