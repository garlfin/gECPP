//
// Created by scion on 11/23/2023.
//

#pragma once

#define GE_MAX_VAO_BUFFER 8
#define GE_MAX_VAO_MATERIAL 8
#define GE_MAX_VAO_FIELD 8
#define GE_MAX_VAO_LOD 8

namespace GPU
{
	enum class ElementType : GLenum
	{
		Byte = 0x1400,
		UByte = 0x1401,
		Short = 0x1402,
		UShort = 0x1403,
		Int = 0x1404,
		UInt = 0x1405,
		Float = 0x1406
	};

	REFLECTABLE_ENUM(Normal, ElementType, 7,
		REFLECT_ENUM(ElementType, Byte),
		REFLECT_ENUM(ElementType, UByte),
		REFLECT_ENUM(ElementType, Short),
		REFLECT_ENUM(ElementType, UShort),
		REFLECT_ENUM(ElementType, Int),
		REFLECT_ENUM(ElementType, UInt),
		REFLECT_ENUM(ElementType, Float),
	);

	struct MaterialSlot : public Serializable<>
	{
		SERIALIZABLE_PROTO("GPU::MaterialSlot", "MAT", 1, MaterialSlot, Serializable);
		REFLECTABLE_NAME_PROTO();

	public:
		MaterialSlot(std::string&& name, u32 offset, u32 count) : Name(move(name)), Offset(offset), Count(count) {}
		MaterialSlot(const std::string& name, u32 offset, u32 count) : Name(name), Offset(offset), Count(count) {}

		std::string Name = DEFAULT;
		u32 Offset = 0;
		u32 Count = 0;
	};

	struct VertexField : public Serializable<>
	{
		SERIALIZABLE_PROTO_NOHEADER("GPU::VertexField", VertexField, Serializable);
		REFLECTABLE_NAME_PROTO();

	public:
		constexpr VertexField(const char name[4], ElementType elementType, bool normalized, u8 bufferIndex, u8 index, u8 elementCount, u8 offset);

		alignas(4) char Name[4] = DEFAULT;
		ElementType ElementType = DEFAULT;
		bool Normalized : 1 = DEFAULT;
		u8 BufferIndex : 7 = DEFAULT;
		u8 Index = DEFAULT;
		u8 ElementCount = DEFAULT;
		u8 Offset = DEFAULT;
	};

	constexpr VertexField::VertexField(const char name[4], enum ElementType elementType, bool normalized, u8 bufferIndex, u8 index, u8 elementCount, u8 offset) :
		ElementType(elementType),
		Normalized(normalized),
		BufferIndex(bufferIndex),
		Index(index),
		ElementCount(elementCount),
		Offset(offset)
	{
		std::copy_n(name, 4, Name);
	}
}
