//
// Created by scion on 8/5/2024.
//

#pragma once

#include "Graphics/Graphics.h"

namespace GPU
{
	template<typename T = u8>
	class Buffer : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(Buffer, Serializable<gE::Window*>);

	 public:
		static_assert(!std::is_pointer_v<T>, "Buffer data shouldn't be a pointer!");

		explicit Buffer(u32 count = 1, const T* data = nullptr) : _array(count, data) {}
		explicit Buffer(const Array<T>& arr) : _array(arr) {}
		explicit Buffer(Array<T>&& arr) : _array(arr) {}

		ALWAYS_INLINE void Free() override { _array.Free(); }
		ALWAYS_INLINE bool IsFree() const override { return _array.IsFree(); }

		GET_CONST(const Array<T>&, Array, _array);
		GET_CONST(u32, Count, _array.Count());
		GET_CONST(const T*, Data, _array.Data())

	 private:
		Array<T> _array;
	};
}

#include "Buffer.inl"

#include <Graphics/API/GL/Buffer/Buffer.h>