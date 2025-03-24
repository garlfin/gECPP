#pragma once

#include <Core/Array.h>
#include <Core/RelativePointer.h>
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

	class IVAO : public GLObject, public Underlying
	{
	public:
		IVAO() = default;
		IVAO(gE::Window* window, GPU::VAO& vao);

		DEFAULT_OPERATOR_CM(IVAO);

		GET(GPU::VAO&, Settings, *_settings);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }
		virtual void Draw(u8 index, u16 instanceCount = 1) const = 0;
		virtual void Draw(u32 count, const GPU::IndirectDraw*) const = 0;
		virtual void DrawDirect(u32 count, u32 offset, u32 instanceCount = 1) const = 0;

		NODISCARD ALWAYS_INLINE const Buffer<std::byte>& GetBuffer(u8 i) const { return _buffers[i]; }
		NODISCARD ALWAYS_INLINE const GPU::VertexField& GetField(u8 i) const { return _settings->Fields[i]; }

		void UpdateBufferDirect(u8 i, std::span<std::byte>, size_t offset = 0) const;
		void UpdateBuffer(u8 i, GPU::Buffer<std::byte>&& buf) const;
		ALWAYS_INLINE void UpdateBuffer(u8 i, GPU::Buffer<std::byte>& buf) const { UpdateBuffer(i, COPY_MOVE(buf)); }

		~IVAO() override;

	private:
		RelativePointer<GPU::VAO> _settings = DEFAULT;
		Array<Buffer<std::byte>> _buffers = DEFAULT;
	};

	class VAO : protected GPU::VAO, public IVAO
	{
		API_SERIALIZABLE(VAO, GPU::VAO);
		API_DEFAULT_CM_CONSTRUCTOR(VAO);
		API_UNDERLYING_IMPL();

	public:
		GET_CONST(const GPU::VAO&, Data, *this);

		GET_CONST(u8, MaterialCount, Counts.MaterialCount);
		GET_CONST(u8, BufferCount, Counts.BufferCount);
		GET_CONST(u8, FieldCount, Counts.FieldCount);

		GET_CONST(const GPU::MaterialSlot*, Materials, Materials);
		GET_CONST(const GPU::Buffer<std::byte>*, Buffers, Buffers);
		GET_CONST(const GPU::VertexField*, Fields, Fields);

		void Draw(u8 index, u16 instanceCount = 1) const override;
		void Draw(u32 count, const GPU::IndirectDraw*) const override;
		void DrawDirect(u32 count, u32 offset, u32 instanceCount = 1) const override;
	};

	class IndexedVAO final : protected GPU::IndexedVAO, public IVAO
	{
		API_SERIALIZABLE(IndexedVAO, GPU::IndexedVAO);
		API_DEFAULT_CM_CONSTRUCTOR(IndexedVAO);
		API_UNDERLYING_IMPL();

	public:
		GET_CONST(GLenum, IndicesFormat, IndicesFormat);
		GET_CONST(const API::Buffer<std::byte>&, Indices, _indicesBuffer);

		void Draw(u8 index, u16 instanceCount = 1) const override;
		void Draw(u32 count, const GPU::IndirectDraw*) const override;
		void DrawDirect(u32 count, u32 offset, u32 instanceCount = 1) const override;

		void UpdateIndicesDirect(std::span<std::byte> data, size_t offset = 0) const;
		void UpdateIndices(GPU::Buffer<std::byte>&& buf) const;
		ALWAYS_INLINE void UpdateIndices(GPU::Buffer<std::byte>& buf) const { UpdateIndices(std::move(GPU::Buffer(buf))); }

	private:
		mutable Buffer<std::byte> _indicesBuffer = DEFAULT;
	};
}
