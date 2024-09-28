//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>

#include "Buffer.h"
#include "VAOSettings.h"

namespace GPU
{
	struct VAOFieldCounts
	{
		u8 MaterialCount : 4;
		u8 BufferCount : 4;
		u8 FieldCount : 4;
	};

	class VAO : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO(VAO, 1, VAO, Serializable);

	public:
		DEFALT_OPERATOR_CM(VAO);

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

	public:
		DEFALT_OPERATOR_CM(IndexedVAO);

		ALWAYS_INLINE void Free() override { VAO::Free(); TriangleBuffer.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VAO::IsFree() && TriangleBuffer.IsFree(); }

		GLenum TriangleFormat = DEFAULT;
		Buffer<u8> TriangleBuffer;

		~IndexedVAO() override { ASSET_CHECK_FREE(IndexedVAO); }
	};
}

#include <Graphics/API/GL/Buffer/VAO.h>