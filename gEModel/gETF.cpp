//
// Created by scion on 8/13/2023.
//

#include <iostream>
#include "gETF.h"

/*
 * void Deserialize(SerializationBuffer& ptr);
 * void Serialize(void*& ptr);
 *
 * 	struct Header;
 * 	struct Mesh;
 * 	struct VertexField;
 * 	struct MaterialSlot;
*/

namespace gETF
{
	void Header::Deserialize(SerializationBuffer& buf) const
	{
		buf.PushPtr("gETF", 4);
		buf.Push<u8>(GETF_VERSION);
		buf.Push(MeshCount);
		buf.PushPtr(Meshes, MeshCount);
	}

	void Header::Serialize(u8*& ptr)
	{
		char magic[4];
		Read<char, 4>(ptr, magic);

		if(!StrCmp<4>(magic, "gETF"))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		u8 version = Read<u8>(ptr);
		MeshCount = Read<u8>(ptr);
		Meshes = new Mesh[MeshCount];
		Read<Mesh>(ptr, Meshes, MeshCount);
	}

	void Mesh::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushPtr("MESH", 4);
		buf.Push(FieldCount);
		buf.PushPtr(Fields, FieldCount);
		buf.Push(TriangleMode);
		buf.Push(MaterialCount);
		buf.PushPtr(Materials, MaterialCount);
	}

	void Mesh::Serialize(u8*& ptr)
	{
		char magic[4];
		Read<char, 4>(ptr, magic);

		if(!StrCmp<4>(magic, "MESH")) std::cout << "Invalid File!\n";

		FieldCount = Read<u8>(ptr);
		Fields = new VertexField[FieldCount];
		Read(ptr, Fields, FieldCount);

		TriangleMode = Read<u8>(ptr);

		MaterialCount = Read<u8>(ptr);
		Materials = new MaterialSlot[MaterialCount];
		Read(ptr, Materials, MaterialCount);
	}

	void VertexField::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushString(Name);
		buf.Push(Index);
		buf.Push(Type);
		buf.Push(TypeCount);
		buf.Push(Count);
		buf.PushPtr((u8*) Data, Count * TypeCount * GetSizeOfGLType(Type));
	}

	void VertexField::Serialize(u8*& ptr)
	{
		Name = ReadString(ptr);
		Index = Read<u8>(ptr);
		Type = Read<u32>(ptr);
		TypeCount = Read<u8>(ptr);
		Count = Read<u32>(ptr);

		size_t bufLen = Count * TypeCount * GetSizeOfGLType(Type);
		Data = new u8[bufLen];
		Read<u8>(ptr, (u8*) Data, bufLen);
	}

	void MaterialSlot::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.Push(MaterialIndex);
		buf.Push(Offset);
		buf.Push(Count);
	}

	void MaterialSlot::Serialize(u8*& ptr)
	{
		MaterialIndex = Read<u8>(ptr);
		Offset = Read<u32>(ptr);
		Count = Read<u32>(ptr);
	}

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count)
	{
		for (u32 i = 0; i < count; i++) PushPtr(t[i].Data(), t[i].Length());
	}

	char* ReadString(u8*& ptr)
	{
		u8 len = Read<u8>(ptr);
		char* str = new char[len + 1];
		Read<char>(ptr, str, len);
		str[len] = 0;
		return str;
	}

	void SerializationBuffer::Realloc(u64 newSize)
	{
		u8* oldBuf = _buf;

		_buf = new u8[newSize] {};
		memcpy(_buf, oldBuf, MIN(newSize, _allocLen));
		_allocLen = newSize;
		delete[] oldBuf;
	}
}

