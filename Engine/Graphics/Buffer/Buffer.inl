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
		if(_version < 2)
			Stride = Read<u8>(in);
		else
			Stride = Read<size_t>(in);
		Data = ReadArray<u32, T>(in);
	}

	template<class T>
	void Buffer<T>::ISerialize(std::ostream& out) const
	{
		Write(out, Stride);
		WriteArray<u32>(out, Data);
	}

	template <typename T>
	Buffer<T>::Buffer(u32 count, const T* data, size_t stride, BufferUsageHint hint, bool createBacking) :
		Stride(stride),
		Data(count, data, createBacking),
		UsageHint(hint)
	{

	}

	template<class T>
	REFLECTABLE_ONGUI_IMPL(Buffer<T>,
	{
		ImGui::TextUnformatted(std::format("Array of {}:\n\tSize: {}\n\tByte Count: {}\n\tPointer: {}",
			demangle(typeid(T).name()),
			Data.Size(),
			Data.ByteSize(),
			(void*) Data.Data()
		).c_str());
	})

	template<class T>
	Buffer<T>* Buffer<T>::Factory(std::istream& in, SETTINGS_T t)
	{
		return new Buffer(in, t);
	};
}
