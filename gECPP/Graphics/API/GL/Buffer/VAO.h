#pragma once

#include "Buffer.h"
#include "../../../../Engine/Utility/Array.h"
#include "Engine/Utility/RelativePointer.h"
#include "Graphics/Buffer/VAO.h"

namespace GL
{
	class IVAO : public GLObject
	{
	public:
		IVAO() = default;
		IVAO(gE::Window* window, GPU::VAO& vao);

		DEFAULT_CM_CONSTRUCTOR(IVAO);

		GET_CONST(const GPU::VAO&, Data, *_settings);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }
		virtual void Draw(u8 index, u16 instanceCount = 1) const = 0;

		NODISCARD ALWAYS_INLINE const Buffer<u8>& GetBuffer(u8 i) { return _buffers[i]; }

	private:
		RelativePointer<GPU::VAO> _settings;
		Array<Buffer<u8>> _buffers;
	};

	class VAO : protected GPU::VAO, public IVAO
	{
		API_SERIALIZABLE(VAO, GPU::VAO);

	 public:
		API_DEFAULT_CM_CONSTRUCTOR(VAO);

		GET_CONST(const GPU::VAO&, Data, *this);

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;

		~VAO() override;
	};

	class IndexedVAO final : protected GPU::IndexedVAO, public IVAO
	{
		API_SERIALIZABLE(IndexedVAO, GPU::IndexedVAO);

	 public:
		API_DEFAULT_CM_CONSTRUCTOR(IndexedVAO);

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
