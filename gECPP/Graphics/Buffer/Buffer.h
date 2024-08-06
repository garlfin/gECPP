//
// Created by scion on 8/5/2024.
//

#pragma once

#include "Graphics/Graphics.h"

namespace gE::GPU
{
	template<typename T = u8>
	class Buffer : public Serializable<Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(Buffer, Serializable<Window*>);

	 public:
		static_assert(!std::is_pointer_v<T>, "Buffer data shouldn't be a pointer!");

		explicit Buffer(u32 count = 1, const T* data = nullptr) : _array(count, data) {}
		explicit Buffer(const Array<T>& arr) : _array(arr) {}
		explicit Buffer(Array<T>&& arr) : _array(arr) {}

		void Free() override { Asset::Free(); _array.Free(); }

		GET_CONST(const Array<T>&, Array, _array);
		GET_CONST(u32, Count, _array.Count());
		GET_CONST(const T*, Data, _array.Data())

	 private:
		Array<T> _array;
	};

	template<class T>
	void Buffer<T>::ISerialize(std::istream& in, Window* s)
	{
		_array = ReadArray<u32, T>(in);
	}

	template<class T>
	void Buffer<T>::IDeserialize(std::ostream& out) const
	{
		WriteArray<u32>(out, _array);
	}
}

#include <Graphics/API/GL/Buffer/Buffer.h>