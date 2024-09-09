#pragma once

#include "GLAD/glad.h"
#include "Graphics/API/GL/GL.h"
#include <type_traits>
#include "Graphics/Macro.h"
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
 	class Buffer : protected GPU::Buffer<T>, public GLObject
	{
		API_SERIALIZABLE(Buffer, GPU::Buffer<T>);

	 public:
		explicit Buffer(gE::Window* window, u32 count = 1, const T* data = nullptr);

		using SUPER::Free;
		using SUPER::IsFree;
		using SUPER::Stride;
		using SUPER::Data;
		using SUPER::Count;
		using SUPER::Data;

		template<typename I>
		ALWAYS_INLINE void ReplaceData(const I* data, uint32_t count = 1, uint32_t offset = 0) const;

		template<typename I>
		void Realloc(uint32_t count, I* data = nullptr);

		ALWAYS_INLINE void Bind(BufferTarget target, uint32_t slot) const;
		ALWAYS_INLINE void Bind(BufferTarget target) const;
		ALWAYS_INLINE void Bind() const override;

		inline ~Buffer() override;
	};

	template<class T>
	using DynamicBuffer = Buffer<T, true>;
}

#include "Buffer.inl"