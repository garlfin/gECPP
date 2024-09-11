#pragma once

#include "Buffer.h"
#include "Engine/Array.h"
#include "Graphics/Buffer/VAO.h"

namespace GL
{
	class IVAO : public GLObject
	{
	public:
		explicit IVAO(gE::Window* window) : GLObject(window) {}

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }
		virtual void Draw(u8 index, u16 instanceCount = 1) const = 0;
	};

	class VAO : protected GPU::VAO, public IVAO
	{
		API_SERIALIZABLE_INIT(VAO, GPU::VAO, IVAO(window));

	 public:
		GET_CONST(u8, MaterialCount, MaterialCount);
		GET_CONST(u8, BufferCount, BufferCount);
		GET_CONST(u8, FieldCount, FieldCount);

		GET_CONST(const GPU::MaterialSlot*, Materials, Materials);
		GET_CONST(const GPU::Buffer<u8>*, Buffers, Buffers);
		GET_CONST(const GPU::VertexField*, Fields, Fields);

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;

		~VAO() override;

	 private:
		Array<Buffer<u8>> _buffers;
	};

	class IndexedVAO final : protected GPU::IndexedVAO, public IVAO
	{
		API_SERIALIZABLE_INIT(IndexedVAO, GPU::IndexedVAO, IVAO(window));

	 public:
		GET_CONST(u8, MaterialCount, MaterialCount);
		GET_CONST(u8, BufferCount, BufferCount);
		GET_CONST(u8, FieldCount, FieldCount);

		GET_CONST(const GPU::MaterialSlot*, Materials, Materials);
		GET_CONST(const GPU::Buffer<u8>*, Buffers, Buffers);
		GET_CONST(const GPU::VertexField*, Fields, Fields);

		GET_CONST(GLenum, TriangleMode, TriangleMode);
		GET_CONST(const GPU::Buffer<u8>&, TriBuffer, TriangleBuffer);

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;

	 private:
		Buffer<u8> _triangleBuffer;
	};
}
