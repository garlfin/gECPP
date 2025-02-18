//
// Created by scion on 11/23/2023.
//

#pragma once

#define GE_MAX_VAO_BUFFER 5
#define GE_MAX_VAO_MATERIAL 5
#define GE_MAX_VAO_FIELD 5
#define GE_MAX_VAO_LOD 8

namespace GPU
{
	struct MaterialSlot : public Serializable<>
	{
		SERIALIZABLE_PROTO("MAT", 1, MaterialSlot, Serializable);

	public:
		MaterialSlot(std::string&& name, u32 offset, u32 count) : Name(move(name)), Offset(offset), Count(count) {}
		MaterialSlot(const std::string& name, u32 offset, u32 count) : Name(name), Offset(offset), Count(count) {}

		std::string Name = DEFAULT;
		u32 Offset = 0;
		u32 Count = 0;
	};

	struct VertexField
	{
		char Name[4] = DEFAULT;
		GLenum ElementType = DEFAULT;
		bool Normalized : 1 = DEFAULT;
		u8 BufferIndex : 7 = DEFAULT;
		u8 Index = DEFAULT;
		u8 ElementCount = DEFAULT;
		u8 Offset = DEFAULT;
	};
}
