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
			glNamedBufferData(ID, SUPER::GetByteCount(), SUPER::GetData(), GetMutableFlags(SUPER::UsageHint));
		else
			glNamedBufferStorage(ID, SUPER::GetByteCount(), SUPER::GetData(), GetImmutableFlags(SUPER::UsageHint));
	}

	template<typename T>
	Buffer<T>::Buffer(gE::Window* window, u32 count, const T* data, GPU::BufferUsageHint hint) :
		APIObject(window)
	{

		static constexpr size_t SIZE = sizeof(typename Array<T>::I);

		SUPER::UsageHint = hint;

		GPU::Buffer<T>::Data = Array<T>(count, (const T*) nullptr, false);

		glCreateBuffers(1, &ID);
		if((bool)(SUPER::UsageHint & GPU::BufferUsageHint::Mutable))
			glNamedBufferData(ID, SIZE * count, data, GetMutableFlags(SUPER::UsageHint));
		else
			glNamedBufferStorage(ID, SIZE * count, data, GetImmutableFlags(SUPER::UsageHint));
	}

	template<typename T>
	template<typename I>
	void Buffer<T>::ReplaceDataDirect(const I* data, uint32_t count, uint32_t offset) const
	{
		static constexpr size_t SIZE = sizeof(typename Array<I>::I);
		GE_ASSERT(SIZE * count + offset <= GPU::Buffer<T>::GetByteCount());

		if(!data || !count) return;
		glNamedBufferSubData(ID, offset, SIZE * count, data);
	}

	template <typename T>
	void Buffer<T>::ReplaceData(Array<T>&& data) const
	{
		GPU::Buffer<T>::Data = std::move(data);
		ReplaceDataDirect(GetSettings().GetData(), GetSettings().GetCount());
	}

	template<typename T>
	void Buffer<T>::Bind(BufferTarget target, uint32_t slot) const
	{
		glBindBufferBase((GLenum) target, slot, ID);
	}

	template<typename T>
	void Buffer<T>::Bind(BufferTarget target) const
	{
		glBindBuffer((GLenum) target, ID);
	}

	template<typename T>
	void Buffer<T>::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	template <typename T>
	void Buffer<T>::RetrieveData(u64 size, u64 offset)
	{
		GE_ASSERTM(!IsFree(), "DATA MAY NOT BE FREE");
		GE_ASSERTM(SUPER::Data.MemSize() > size + offset, "DATA ARRAY NOT LARGE ENOUGH TO STORE MEMORY");
		RetrieveData(SUPER::Data.Data(), size, offset);
	}

	template <typename T>
	void Buffer<T>::RetrieveData(T* data, u64 size, u64 offset)
	{
		glGetNamedBufferSubData(ID, offset, size, data);
	}

	template<typename T>
	template<typename I>
	void Buffer<T>::Realloc(uint32_t count, I* data)
	{
		static constexpr size_t SIZE_T = sizeof(typename Array<I>::I);
		if((bool)(SUPER::UsageHint & GPU::BufferUsageHint::Mutable))
			glNamedBufferData(ID, SIZE_T * count, data, GetMutableFlags(SUPER::UsageHint));
		else
		{
			LOG("Consider using Buffer<T, true> (Dynamic Buffer) when reallocating.");
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
