//
// Created by scion on 11/23/2023.
//

#pragma once

#define GE_MAX_VAO_BUFFER 5
#define GE_MAX_VAO_MATERIAL 5
#define GE_MAX_VAO_FIELD 5

namespace GPU
{
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
}
