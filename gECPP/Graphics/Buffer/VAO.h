//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>

#include "Buffer.h"
#include "VAOSettings.h"

namespace GPU
{
	class VAO : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(VAO, Serializable);

	public:
		u8 MaterialCount : 4;
		u8 BufferCount : 4;
		u8 FieldCount : 4;

		MaterialSlot Materials[GE_MAX_VAO_MATERIAL];
		Buffer<u8> Buffers[GE_MAX_VAO_BUFFER];
		VertexField Fields[GE_MAX_VAO_FIELD];

		void Free() override { for(Buffer<u8>& buffer : Buffers) buffer.Free(); };

		NODISCARD bool IsFree() const override
		{
			for(const Buffer<u8>& buffer : Buffers)
				if(!buffer.IsFree())
					return false;
			return true;
		}
	};

	class IndexedVAO : public VAO
	{
		SERIALIZABLE_PROTO_T(IndexedVAO, VAO);

	public:
		ALWAYS_INLINE void Free() override { VAO::Free(); TriangleBuffer.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VAO::IsFree() && TriangleBuffer.IsFree(); }

		GLenum TriangleMode;
		Buffer<u8> TriangleBuffer;
	};
}

#include <Graphics/API/GL/Buffer/VAO.h>