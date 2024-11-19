#pragma once

#include <Engine/Utility/Array.h>
#include <Engine/Utility/Manager.h>
#include <Engine/Utility/RelativePointer.h>
#include <Graphics/Buffer/VAO.h>
#include "Buffer.h"

namespace GL
{
	struct IndirectDraw
	{
		u32 Count;
		u32 InstanceCount;
		u32 First;
		u32 BaseInstance;
	};

	struct IndirectDrawIndexed
	{
		u32 Count;
		u32 InstanceCount;
		u32 FirstIndex;
		u32 BaseVertex;
		u32 BaseInstance;
	};

	class IVAO : public GLObject
	{
	public:
		IVAO() = default;
		IVAO(gE::Window* window, GPU::VAO& vao);

		DEFAULT_OPERATOR_CM(IVAO);

		GET_CONST(const GPU::VAO&, Data, *_settings);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }
		virtual void Draw(u8 index, u16 instanceCount = 1) const = 0;
		virtual void Draw(u32 count, const GPU::IndirectDraw*) const = 0;

		NODISCARD ALWAYS_INLINE const Buffer<u8>& GetBuffer(u8 i) { return _buffers[i]; }

	private:
		RelativePointer<GPU::VAO> _settings = DEFAULT;
		Array<Buffer<u8>> _buffers = DEFAULT;
	};

	class VAO : protected GPU::VAO, public IVAO
	{
		API_SERIALIZABLE(VAO, GPU::VAO);
		API_DEFAULT_CM_CONSTRUCTOR(VAO);
		API_UNDERLYING_IMPL(IVAO);

	 public:
		GET_CONST(const GPU::VAO&, Data, *this);

		using SUPER::Free;
		using SUPER::IsFree;

		void Draw(u8 index, u16 instanceCount = 1) const override;
		void Draw(u32 count, const GPU::IndirectDraw*) const override;

		~VAO() override;
	};

	class IndexedVAO final : protected GPU::IndexedVAO, public IVAO
	{
		API_SERIALIZABLE(IndexedVAO, GPU::IndexedVAO);
		API_DEFAULT_CM_CONSTRUCTOR(IndexedVAO);
		API_UNDERLYING_IMPL(IVAO);

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
		void Draw(u32 count, const GPU::IndirectDraw*) const override;

	 private:
		Buffer<u8> _triangleBuffer = DEFAULT;
	};
}
