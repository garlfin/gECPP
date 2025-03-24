//
// Created by scion on 9/7/2024.
//

#pragma once

#include "Buffer.h"

namespace GL
{
	template<typename T>
	Buffer<T>::Buffer(gE::Window* window, GPU::Buffer<T>&& INTERNAL_SETTINGS) :
		SUPER(move(INTERNAL_SETTINGS)), APIObject(window)
	{
		glCreateBuffers(1, &ID);

		if((bool)(SUPER::UsageHint & GPU::BufferUsageHint::Mutable))
			glNamedBufferData(ID, SUPER::GetByteSize(), SUPER::GetData(), GetMutableFlags(SUPER::UsageHint));
		else
			glNamedBufferStorage(ID, SUPER::GetByteSize(), SUPER::GetData(), GetImmutableFlags(SUPER::UsageHint));
	}

	template<typename T>
	Buffer<T>::Buffer(gE::Window* window, size_t count, const T* data, GPU::BufferUsageHint hint, bool createBacking) :
		Buffer(window, SUPER(count, data, sizeof(typename Array<T>::I), hint, createBacking))
	{
	}

	template <typename T>
	void Buffer<T>::ReplaceData(Array<T>&& data)
	{
		GPU::Buffer<T>::Data = std::move(data);
		UpdateDataDirect(data.begin(), data.end());
	}

	template <typename T>
	void Buffer<T>::ReplaceData(Array<T>& data)
	{
		ReplaceData(std::move(Array(data)));
	}

	template <typename T>
	template <typename I>
	void Buffer<T>::UpdateDataDirect(std::span<I> data, size_t offset) const
	{
		GE_ASSERTM(data.size_bytes() + offset <= GPU::Buffer<T>::GetByteSize(), "Too much data for buffer.");

		if(!data.size()) return;
		glNamedBufferSubData(ID, offset, data.size_bytes(), data.data());
	}

	template <typename T>
	template <typename I>
	void Buffer<T>::UpdateData(size_t count, size_t offset) const
	{
		if(!count) count = (SUPER::Data.ByteSize() - offset) / sizeof(I);

		std::byte* const beg = (std::byte*) GetData().begin() + offset;
		std::byte* const end = beg + count * sizeof(I);

		GE_ASSERTM(sizeof(I) * count + offset <= GPU::Buffer<T>::GetByteSize(), "Too much data for buffer.");
		UpdateDataDirect(std::span((I*) beg, (I*) end), offset);
	}

	template <typename T>
	template <typename I>
	void Buffer<T>::RetrieveDataDirect(std::span<I> data, size_t offset) const
	{
		GE_ASSERTM(data.size_bytes() + offset <= SUPER::Data.ByteSize(), "DATA ARRAY NOT LARGE ENOUGH TO STORE MEMORY");
		glGetNamedBufferSubData(ID, offset, data.size_bytes(), data.data());
	}

	template <typename T>
	template <typename I>
	void Buffer<T>::RetrieveData(size_t count, size_t offset)
	{
		if(!count) count = (SUPER::Data.ByteSize() - offset) / sizeof(I);

		std::byte* const beg = (std::byte*) GetData().begin() + offset;
		std::byte* const end = beg + count * sizeof(I);

		GE_ASSERT(!IsFree());
		GE_ASSERTM(sizeof(I) * count + offset <= SUPER::Data.ByteSize(), "DATA ARRAY NOT LARGE ENOUGH TO STORE MEMORY");
		RetrieveDataDirect<I>(std::span((I*) beg, (I*) end), offset);
	}

	template<typename T>
	void Buffer<T>::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	template<typename T>
	void Buffer<T>::Bind(BufferBaseTarget target, uint32_t slot) const
	{
		glBindBufferBase((GLenum) target, slot, ID);
	}

	template<typename T>
	void Buffer<T>::Bind(BufferTarget target) const
	{
		glBindBuffer((GLenum) target, ID);
	}

	template<typename T>
	template<typename I>
	void Buffer<T>::Reallocate(uint32_t count, I* data)
	{
		static constexpr size_t SIZE_T = sizeof(typename Array<I>::I);
		if((bool)(SUPER::UsageHint & GPU::BufferUsageHint::Mutable))
			glNamedBufferData(ID, SIZE_T * count, data, GetMutableFlags(SUPER::UsageHint));
		else
		{
			gE::Log::Write("Consider using a buffer with the dynamic flag when reallocating.");
			glDeleteBuffers(1, &ID);
			glCreateBuffers(1, &ID);
			glNamedBufferStorage(ID, SIZE_T * count, data, GetImmutableFlags(SUPER::UsageHint));
		}
	}

	template<typename T>
	Buffer<T>::~Buffer()
	{
		glDeleteBuffers(1, &ID);
	}

	template <typename T>
	u32 Buffer<T>::GetMutableFlags(GPU::BufferUsageHint hint)
	{
		return (bool)(hint & GPU::BufferUsageHint::Dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	}

	template <typename T>
	u32 Buffer<T>::GetImmutableFlags(GPU::BufferUsageHint hint)
	{
		u32 result = 0;
		if((bool)(hint & GPU::BufferUsageHint::Dynamic)) result |= GL_DYNAMIC_STORAGE_BIT;
		if((bool)(hint & GPU::BufferUsageHint::Read)) result |= GL_MAP_READ_BIT;
		if((bool)(hint & GPU::BufferUsageHint::Write)) result |= GL_MAP_WRITE_BIT;

		return result;
	}
}
