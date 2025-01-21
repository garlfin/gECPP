//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>
#include <Serializable/Macro.h>

namespace GL
{
	template<class T>
	class Buffer;
}

namespace GPU
{
	enum class BufferUsageHint
	{
		Mutable = 1,
		Dynamic = 1 << 1,
		Read = 1 << 2,
		Write = 1 << 3,
		Default = 0
	};

	ENUM_OPERATOR(BufferUsageHint, |);
	ENUM_OPERATOR(BufferUsageHint, &);

	template<typename T = void>
	class Buffer : public gE::Asset
	{
		SERIALIZABLE_PROTO("SBUF", 1, Buffer, Asset);
		SERIALIZABLE_REFLECTABLE(Buffer, "GPU::Buffer");

	 public:
		static_assert(!std::is_pointer_v<T>, "Buffer data shouldn't be a pointer!");

		explicit Buffer(u32 count, const T* data = nullptr) : Stride(sizeof(typename Array<T>::I)), Data(data, count) {}
		explicit Buffer(const Array<T>& arr) : Stride(sizeof(typename Array<T>::I)), Data(arr) {}
		explicit Buffer(Array<T>&& arr) : Stride(sizeof(typename Array<T>::I)), Data(arr) {}

		GET_CONST(u8, Stride, Stride);
		GET_CONST(const Array<T>&, Array, Data);
		GET_CONST(const T*, Data, Data.Data())
		GET_CONST(u32, ElementCount, Data.Count() / Stride);
		GET_CONST(u32, Count, Data.Count());
		GET_CONST(u32, ByteCount, Data.ByteCount());

		ALWAYS_INLINE void Free() override { Data.Free(); }
		ALWAYS_INLINE bool IsFree() const override { return Data.IsFree(); }

		uint8_t Stride = DEFAULT;
		Array<T> Data = DEFAULT;
		BufferUsageHint UsageHint = BufferUsageHint::Default;

		~Buffer() override { ASSET_CHECK_FREE(Buffer); }
	};
}

#include "Buffer.inl"

#include <Graphics/API/GL/Buffer/Buffer.h>

template <typename T>
GPU::Buffer<T>* GPU::Buffer<T>::BufferFACTORY(std::istream& in, SETTINGS_T t)
{
	return (Buffer*) new API::Buffer<T>(in, t);
}
