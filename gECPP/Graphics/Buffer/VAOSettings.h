//
// Created by scion on 11/23/2023.
//

#pragma once

#define GE_MAX_VAO_BUFFER 5
#define GE_MAX_VAO_MATERIAL 5
#define GE_MAX_VAO_FIELD 5

namespace GPU
{
	struct MaterialSlot : public Serializable<>
	{
		SERIALIZABLE_PROTO(MaterialSlot, Serializable);
		DEFAULT_CM_CONSTRUCTOR(MaterialSlot);

	public:
		MaterialSlot(std::string&& name, u32 offset, u32 count) : Name(MOVE(name)), Offset(offset), Count(count) {}
		MaterialSlot(const std::string& name, u32 offset, u32 count) : Name(name), Offset(offset), Count(count) {}

		std::string Name;
		u32 Offset = 0;
		u32 Count = 0;
	};

	struct VertexField
	{
		char Name[4];
		GLenum ElementType;
		bool Normalized : 1;
		u8 BufferIndex : 7;
		u8 Index;
		u8 ElementCount;
		u8 Offset;
	};
}
