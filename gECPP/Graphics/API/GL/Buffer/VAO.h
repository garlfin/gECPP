#pragma once

#include "Buffer.h"
#include "Engine/Array.h"
#include "Graphics/Buffer/VAO.h"

namespace GL
{
	class IVAO : public GLObject
	{
	public:
		IVAO() = default;
		explicit IVAO(gE::Window* window, GPU::VAO& vao) : GLObject(window),
			_vao(&vao) {}

		GET_CONST(const GPU::VAO&, Data, *_vao);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }
		virtual void Draw(u8 index, u16 instanceCount = 1) const = 0;

	private:
		GPU::VAO* _vao;
	};

	class VAO : protected GPU::VAO, public IVAO
	{
		API_SERIALIZABLE_INIT(VAO, GPU::VAO, IVAO(window, *this));
		API_DEFAULT_CM_CONSTRUCTOR(VAO);

	 public:
		GET_CONST(const GPU::VAO&, Data, *this);

		NODISCARD ALWAYS_INLINE const Buffer<u8>& GetBuffer(u8 i) { return _buffers[i]; }

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;

		~VAO() override;

	 private:
		Array<Buffer<u8>> _buffers;
	};

	class IndexedVAO final : protected GPU::IndexedVAO, public IVAO
	{
		API_SERIALIZABLE_INIT(IndexedVAO, GPU::IndexedVAO, IVAO(window, *this));
		API_DEFAULT_CM_CONSTRUCTOR(IndexedVAO);

	 public:
		GET_CONST(u8, MaterialCount, Counts.MaterialCount);
		GET_CONST(u8, BufferCount, Counts.BufferCount);
		GET_CONST(u8, FieldCount, Counts.FieldCount);

		GET_CONST(const GPU::MaterialSlot*, Materials, Materials);
		GET_CONST(const GPU::Buffer<u8>*, Buffers, Buffers);
		GET_CONST(const GPU::VertexField*, Fields, Fields);

		GET_CONST(GLenum, TriangleMode, TriangleFormat);
		GET_CONST(const GPU::Buffer<u8>&, TriBuffer, TriangleBuffer);

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;

	 private:
		Buffer<u8> _triangleBuffer;
	};
}
