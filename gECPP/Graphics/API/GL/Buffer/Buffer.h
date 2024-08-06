#pragma once

#include "GLAD/glad.h"
#include "Graphics/API/GL/GL.h"
#include <type_traits>
#include <iostream>
#include "VAOSettings.h"
#include "Graphics/Buffer/Buffer.h"

namespace GL
{
	enum class BufferTarget
	{
		Atomic = GL_ATOMIC_COUNTER_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER,
		Buffer = GL_ARRAY_BUFFER
	};

	template<typename T = u8, bool DYNAMIC = false>
 	class Buffer : public gE::Graphics::Buffer<T>, public GLObject
	{
	 public:
		explicit Buffer(gE::Window* window, u32 count = 1, const T* data = nullptr) :
			gE::Graphics::Buffer<T>(count, data) , GLObject(window)
		{ Construct(); }

		explicit Buffer(gE::Window* window, const Array<T>& arr) :
			gE::Graphics::Buffer<T>(arr) , GLObject(window)
		{ Construct(); }

		explicit Buffer(gE::Window* window, Array<T>&& arr) :
			gE::Graphics::Buffer<T>(std::move(arr)) , GLObject(window)
		{ Construct(); }

		template<typename I>
		ALWAYS_INLINE void ReplaceData(const I* data, uint32_t count = 1, uint32_t offset = 0) const
		{
			static constexpr size_t SIZE_T = sizeof(std::conditional_t<std::is_same_v<I, void>, uint8_t, I>);
			if(!data || !count) return;
			glNamedBufferSubData(ID, offset, SIZE_T * count, data);
		}

		ALWAYS_INLINE void Bind(BufferTarget target, uint32_t slot) const
		{
			glBindBufferBase((GLenum) target, slot, ID);
		}

		ALWAYS_INLINE void Bind(BufferTarget target) const
		{
			glBindBuffer((GLenum) target, ID);
		}

		ALWAYS_INLINE void Bind() const override
		{
			glBindBuffer(GL_ARRAY_BUFFER, ID);
		}

		template<typename I>
		void Realloc(uint32_t count, I* data = nullptr) const
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

		inline ~Buffer() override
		{
			glDeleteBuffers(1, &ID);
		}

	 private:
		void Construct()
		{
			typedef gE::Graphics::Buffer<T> S;
			static constexpr size_t SIZE_T = sizeof(std::conditional_t<std::is_same_v<T, void>, uint8_t, T>);

			glCreateBuffers(1, &ID);

			if constexpr(DYNAMIC) glNamedBufferData(ID, SIZE_T * S::GetCount(), S::GetData(), GL_DYNAMIC_DRAW);
			else glNamedBufferStorage(ID, SIZE_T * S::GetCount(), S::GetData(), GL_DYNAMIC_STORAGE_BIT);
		}
	};

	template<class T>
	using DynamicBuffer = Buffer<T, true>;
}