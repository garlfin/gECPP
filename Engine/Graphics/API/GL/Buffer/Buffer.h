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

	enum class BufferBaseTarget
	{
		Atomic = GL_ATOMIC_COUNTER_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER
	};

	template<typename T = void>
 	class Buffer : protected GPU::Buffer<T>, public GLObject
	{
		API_SERIALIZABLE(Buffer, GPU::Buffer<T>);
		API_DEFAULT_CM_CONSTRUCTOR(Buffer);

	public:
		Buffer(gE::Window* window, size_t count, const T* data = nullptr, GPU::BufferUsageHint hint = GPU::BufferUsageHint::Default, bool createBacking = false);

		GET_CONST(const Array<T>&, Data, GPU::Buffer<T>::Data);
		void ReplaceData(Array<T>&&); // Updates CPU-side buffer
		void ReplaceData(Array<T>& data); // Updates CPU-side buffer

		template<typename I = T>
		void UpdateDataDirect(std::span<I> data, size_t offset = 0) const; // CPU to GPU, without using CPU-side buffer
		template<typename I = T>
		void UpdateData(size_t count = 0, size_t offset = 0) const; // CPU to GPU
		template<typename I = T>
		void RetrieveDataDirect(std::span<I> data, size_t offset = 0) const;
		template<typename I = T>
		void RetrieveData(size_t count = 0, size_t offset = 0);

		template<typename I>
		void Reallocate(uint32_t count, I* data = nullptr);

		void Bind(BufferBaseTarget target, uint32_t slot) const;
		void Bind(BufferTarget target) const;
		void Bind() const override;

		~Buffer() override;

	private:
		static u32 GetMutableFlags(GPU::BufferUsageHint);
		static u32 GetImmutableFlags(GPU::BufferUsageHint);
	};
}

#include "Buffer.inl"