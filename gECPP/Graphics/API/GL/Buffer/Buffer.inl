//
// Created by scion on 9/7/2024.
//

#pragma once

#include "Buffer.h"

namespace GL
{
	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::Buffer(gE::Window* window, GPU::Buffer<T>&& settings) :
		SUPER(MOVE(settings)), APIObject(window)
	{
		glCreateBuffers(1, &ID);

		if constexpr(DYNAMIC)
			glNamedBufferData(ID, GetByteCount(), GetData(), GL_DYNAMIC_DRAW);
		else
			glNamedBufferStorage(ID, GetByteCount(), GetData(), GL_DYNAMIC_STORAGE_BIT);
	}

	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::Buffer(gE::Window* window, u32 count, const T* data) :
		APIObject(window)
	{
		static constexpr size_t SIZE_T = sizeof(typename Array<T>::I);

		glCreateBuffers(1, &ID);

		if constexpr(DYNAMIC) glNamedBufferData(ID, SIZE_T * count, data, GL_DYNAMIC_DRAW);
		else glNamedBufferStorage(ID, SIZE_T * count, data, GL_DYNAMIC_STORAGE_BIT);
	}

	template<typename T, bool DYNAMIC>
	template<typename I>
	void Buffer<T, DYNAMIC>::ReplaceData(const I* data, uint32_t count, uint32_t offset) const
	{
		static constexpr size_t SIZE_T = sizeof(std::conditional_t<std::is_same_v<I, void>, uint8_t, I>);
		if(!data || !count) return;
		glNamedBufferSubData(ID, offset, SIZE_T * count, data);
	}

	template<typename T, bool DYNAMIC>
	void Buffer<T, DYNAMIC>::Bind(BufferTarget target, uint32_t slot) const
	{
		glBindBufferBase((GLenum) target, slot, ID);
	}

	template<typename T, bool DYNAMIC>
	void Buffer<T, DYNAMIC>::Bind(BufferTarget target) const
	{
		glBindBuffer((GLenum) target, ID);
	}

	template<typename T, bool DYNAMIC>
	void Buffer<T, DYNAMIC>::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	template<typename T, bool DYNAMIC>
	template<typename I>
	void Buffer<T, DYNAMIC>::Realloc(uint32_t count, I* data)
	{
		static constexpr size_t SIZE_T = sizeof(std::conditional_t<std::is_same_v<I, void>, uint8_t, I>);
		if constexpr(DYNAMIC) glNamedBufferData(ID, SIZE_T * count, data, GL_DYNAMIC_DRAW);
		else
		{
			LOG("Consider using Buffer<T, true> (Dynamic Buffer) when reallocating.");
			glDeleteBuffers(1, &ID);
			glCreateBuffers(1, &ID);
			glNamedBufferStorage(ID, SIZE_T * count, data, GL_DYNAMIC_STORAGE_BIT);
		}
	}

	template<typename T, bool DYNAMIC>
	Buffer<T, DYNAMIC>::~Buffer()
	{
		glDeleteBuffers(1, &ID);
	}
}
