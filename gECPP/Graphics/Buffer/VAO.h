//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>

#include "Buffer.h"
#include "VAOSettings.h"

#define API_MAX_MULTI_DRAW 8

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

	class VAO : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(VAO, 1, VAO, Serializable);
		API_REFLECTABLE(VAO, "GPU::VAO", API::VAO);
		API_UNDERLYING();

	public:
		DEFAULT_OPERATOR_CM(VAO);

		VAOFieldCounts Counts{};
		MaterialSlot Materials[GE_MAX_VAO_MATERIAL];
		VertexField Fields[GE_MAX_VAO_FIELD];
		Buffer<u8> Buffers[GE_MAX_VAO_BUFFER];

		void Free() override { for(Buffer<u8>& buffer : Buffers) buffer.Free(); };

		NODISCARD bool IsFree() const override
		{
			for(const Buffer<u8>& buffer : Buffers)
				if(!buffer.IsFree())
					return false;
			return true;
		}

		~VAO() override { ASSET_CHECK_FREE(VAO); }
	};

	class IndexedVAO : public VAO
	{
		SERIALIZABLE_PROTO(IVAO, 1, IndexedVAO, VAO);
		API_REFLECTABLE(IndexedVAO, "GPU::IndexedVAO", API::IndexedVAO);

	public:
		DEFAULT_OPERATOR_CM(IndexedVAO);

		ALWAYS_INLINE void Free() override { VAO::Free(); TriangleBuffer.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VAO::IsFree() && TriangleBuffer.IsFree(); }

		GLenum TriangleFormat = DEFAULT;
		Buffer<u8> TriangleBuffer;

		~IndexedVAO() override { ASSET_CHECK_FREE(IndexedVAO); }
	};
}

#include <Graphics/API/GL/Buffer/VAO.h>