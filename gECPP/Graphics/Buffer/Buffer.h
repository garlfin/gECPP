//
// Created by scion on 8/5/2024.
//

#pragma once

#include "Graphics/Graphics.h"

namespace GPU
{
	template<typename T = void>
	class Buffer : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO(SBUF, 1, Buffer, Serializable<gE::Window*>);

	 public:
		static_assert(!std::is_pointer_v<T>, "Buffer data shouldn't be a pointer!");

		explicit Buffer(u32 count, const T* data = nullptr) : Stride(sizeof(typename Array<T>::I)), Data(count, data) {}
		explicit Buffer(const Array<T>& arr) : Stride(sizeof(typename Array<T>::I)), Data(arr) {}
		explicit Buffer(Array<T>&& arr) : Stride(sizeof(typename Array<T>::I)), Data(arr) {}

		DEFAULT_CM_CONSTRUCTOR(Buffer);

		GET_CONST(u8, Stride, Stride);
		GET_CONST(const Array<T>&, Array, Data);
		GET_CONST(const T*, Data, Data.Data())
		GET_CONST(u32, ElementCount, Data.Count() / Stride);
		GET_CONST(u32, Count, Data.Count());
		GET_CONST(u32, ByteCount, sizeof(typename Array<T>::I) * Data.Count());

		ALWAYS_INLINE void Free() override { Data.Free(); }
		ALWAYS_INLINE bool IsFree() const override { return Data.IsFree(); }

		uint8_t Stride;
		Array<T> Data;

		~Buffer() override { ASSET_CHECK_FREE(Buffer); }
	};
}

#include "Buffer.inl"

#include <Graphics/API/GL/Buffer/Buffer.h>