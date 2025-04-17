//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>

#include "Buffer.h"
#include "VAOSettings.h"

namespace GL
{
	class VAO;
	class IndexedVAO;
}

namespace GPU
{
	struct VAOFieldCounts
	{
		u8 MaterialCount : 4;
		u8 BufferCount : 4;
		u8 FieldCount : 4;
	};

	struct IndirectDraw
	{
		u32 InstanceCount;
		u8 Material;
		u8 LOD;
	};

	enum class PrimitiveType
	{
		Point = 1,
		Line = 2,
		Triangle = 3,
	};

	NODISCARD GLenum ToGLEnum(PrimitiveType primType);

	class VAO : public gE::Asset
	{
		SERIALIZABLE_PROTO("VAO", 1, VAO, Asset);

	public:
		VAOFieldCounts Counts{};
		MaterialSlot Materials[GE_MAX_VAO_MATERIAL];
		VertexField Fields[GE_MAX_VAO_FIELD];
		Buffer<std::byte> Buffers[GE_MAX_VAO_BUFFER];
		PrimitiveType PrimitiveType = PrimitiveType::Triangle;

		void Free() override { for(Buffer<std::byte>& buffer : Buffers) buffer.Free(); };
		void AddField(const VertexField& field) { GE_ASSERT(Counts.FieldCount < GE_MAX_VAO_FIELD); Fields[Counts.FieldCount++] = field; }
		void AddBuffer(Buffer<std::byte>&& buf) { GE_ASSERT(Counts.BufferCount < GE_MAX_VAO_BUFFER); Buffers[Counts.BufferCount++] = buf; }
		void AddMaterial(MaterialSlot&& material) { GE_ASSERT(Counts.BufferCount < GE_MAX_VAO_MATERIAL); Materials[Counts.MaterialCount++] = material; }

		NODISCARD bool IsFree() const override
		{
			for(const Buffer<std::byte>& buffer : Buffers)
				if(!buffer.IsFree())
					return false;
			return true;
		}

		~VAO() override { ASSET_CHECK_FREE(VAO); }
	};

	class IndexedVAO : public VAO
	{
		SERIALIZABLE_PROTO("IVAO", 1, IndexedVAO, VAO, &GPU::VAO::SType);

	public:
		ALWAYS_INLINE void Free() override { VAO::Free(); IndicesBuffer.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VAO::IsFree() && IndicesBuffer.IsFree(); }

		GLenum IndicesFormat = DEFAULT;
		Buffer<std::byte> IndicesBuffer;

		~IndexedVAO() override { ASSET_CHECK_FREE(IndexedVAO); }
	};
}

#if API == GL
	#include <Graphics/API/GL/Buffer/VAO.h>
#endif