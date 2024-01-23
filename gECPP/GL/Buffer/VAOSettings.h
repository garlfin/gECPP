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
		u32 Count;
		void* Data;
	};

	struct MaterialSlot
	{
		u32 Offset = 0;
		u32 Count = 0;
	};

	struct VertexField
	{
		GLenum ElementType;
		bool Normalized : 1;
		u8 BufferIndex : 7;
		u8 Index;
		u8 ElementCount;
		u8 Offset;
	};

	struct VAOSettings
	{
		u8 BufferCount;
		BufferSettings Buffers[GE_MAX_VAO_BUFFER];

		u8 FieldCount;
		VertexField Fields[GE_MAX_VAO_FIELD];

		u8 MaterialCount;
		MaterialSlot Materials[GE_MAX_VAO_MATERIAL];
	};

	struct IndexedVAOSettings : public VAOSettings
	{
		VertexField Triangles;
	};
}