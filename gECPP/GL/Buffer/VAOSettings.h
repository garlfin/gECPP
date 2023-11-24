//
// Created by scion on 11/23/2023.
//

#pragma once

#include <GL/Binary/Binary.h>

#define GE_MAX_VAO_BUFFER 5
#define GE_MAX_VAO_MATERIAL 5
#define GE_MAX_VAO_FIELD 5

namespace GL
{
	struct BufferSettings
	{
		u8 Stride;
		u32 Length;
		void* Data;
	};

	struct MaterialSlot
	{
		u8 MaterialIndex = 0;
		u32 Offset = 0;
		u32 Count = 0;
	};

	struct VertexField
	{
		GLenum ElementType;
		u8 BufferIndex;
		u8 Index;
		u8 ElementCount;
		u8 Offset;
	};

	// TODO: Stop being LAZY (!!!) and just use dynamic arrays.
	struct VAOSettings
	{
		u8 BufferCount;
		BufferSettings Buffers[GE_MAX_VAO_BUFFER];

		u8 MaterialCount;
		MaterialSlot Materials[GE_MAX_VAO_MATERIAL];

		u8 FieldCount;
		VertexField Fields[GE_MAX_VAO_FIELD];
	};

	struct IndexedVAOSettings : public VAOSettings
	{
		VertexField Triangles;
	};
}