//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>
#include <Serializable/Macro.h>

#include "Window/KeyboardState.h"

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

	template<typename T = u8>
	class Buffer : public gE::Asset
	{
		SERIALIZABLE_PROTO("SBUF", 1, Buffer, Asset);
		REFLECTABLE_PROTO(Buffer);

	 public:
		static_assert(!std::is_pointer_v<T>, "Buffer data shouldn't be a pointer!");

		explicit Buffer(u32 count, const T* = nullptr, u8 stride = sizeof(typename Array<T>::I), bool createBacking = true);
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

namespace GPU
{
	template <typename T>
	Buffer<T>* Buffer<T>::BufferFACTORY(std::istream& in, SETTINGS_T t)
	{
		return (Buffer*) new API::Buffer<T>(in, t);
	}

	FORCE_IMPL static ITypeSystem<Buffer<>::SETTINGS_T>::Type _REFL_IMPL_TYPE_Buffer{ "GPU::Buffer", (ITypeSystem<Buffer<>::SETTINGS_T>::FactoryFunction) Buffer<>::BufferFACTORY, {  }};

	template<class T>
	const ITypeSystem<typename Buffer<T>::SETTINGS_T>::Type& Buffer<T>::SGetType()
	{
		return _REFL_IMPL_TYPE_Buffer;
	};
}

