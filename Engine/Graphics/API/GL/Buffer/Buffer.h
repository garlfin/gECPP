#pragma once

#include <Core/Serializable/Macro.h>
#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Buffer/Buffer.h>

namespace GL
{
	enum class BufferTarget
	{
		Atomic = GL_ATOMIC_COUNTER_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER,
		Buffer = GL_ARRAY_BUFFER,
		IndirectDrawBuffer = GL_DRAW_INDIRECT_BUFFER
	};

	template<typename T = void>
 	class Buffer : protected GPU::Buffer<T>, public GLObject
	{
		API_SERIALIZABLE(Buffer, GPU::Buffer<T>);
		API_DEFAULT_CM_CONSTRUCTOR(Buffer);

	public:
		Buffer(gE::Window* window, u32 count, const T* data = nullptr, GPU::BufferUsageHint hint = GPU::BufferUsageHint::Default);

		template<typename I>
		void ReplaceDataDirect(const I* data, uint32_t count = 1, uint32_t offset = 0) const;

		void ReplaceData(Array<T>&&) const;
		ALWAYS_INLINE void ReplaceData(Array<T>& data) const { ReplaceData(std::move(Array(data))); }

		template<typename I>
		void Realloc(uint32_t count, I* data = nullptr);

		ALWAYS_INLINE void Bind(BufferTarget target, uint32_t slot) const;
		ALWAYS_INLINE void Bind(BufferTarget target) const;
		ALWAYS_INLINE void Bind() const override;

		void RetrieveData(u64 size = sizeof(T), u64 offset = 0);
		void RetrieveData(T* data, u64 size = sizeof(T), u64 offset = 0);

		inline ~Buffer() override;

	private:
		static u32 GetMutableFlags(GPU::BufferUsageHint);
		static u32 GetImmutableFlags(GPU::BufferUsageHint);
	};
}

#include "Buffer.inl"