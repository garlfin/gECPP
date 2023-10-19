//
// Created by scion on 8/13/2023.
//

#include <iostream>
#include "../gETF.h"
#include "GL/Binary.h"
#include "File.h"
#include "GL/Buffer/VAO.h"


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
		::Read<char, 4>(ptr, magic);

		if(!StrCmp<4>(magic, "gETF"))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		u8 version = ::Read<u8>(ptr);
		MeshCount = ::Read<u8>(ptr);
		Meshes = new Mesh[MeshCount];
		::Read<Mesh>(ptr, Meshes, MeshCount);
	}

	void Mesh::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushPtr("MESH", 4);
		buf.Push(FieldCount);
		buf.PushPtr(Fields, FieldCount);
		buf.Push(TriangleMode);
		if(TriangleMode == TriangleMode::Simple) buf.Push(Triangles);
		buf.Push(MaterialCount);
		buf.PushPtr(Materials, MaterialCount);
	}

	void Mesh::Serialize(u8*& ptr)
	{
		char magic[4];
		::Read<char, 4>(ptr, magic);

		if(!StrCmp<4>(magic, "MESH")) std::cout << "Invalid File!\n";

		FieldCount = ::Read<u8>(ptr);
		Fields = new VertexField[FieldCount];
		::Read(ptr, Fields, FieldCount);

		TriangleMode = ::Read<enum TriangleMode>(ptr);
		if(TriangleMode == TriangleMode::Simple) Triangles.Serialize(ptr);

		MaterialCount = ::Read<u8>(ptr);
		Materials = new MaterialSlot[MaterialCount];
		::Read(ptr, Materials, MaterialCount);
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
		Index = ::Read<u8>(ptr);
		Type = ::Read<u32>(ptr);
		TypeCount = ::Read<u8>(ptr);
		Count = ::Read<u32>(ptr);

		size_t bufLen = Count * TypeCount * GetSizeOfGLType(Type);
		Data = malloc(bufLen);
		::Read<u8>(ptr, (u8*) Data, bufLen);
	}

	void MaterialSlot::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.Push(MaterialIndex);
		buf.Push(Offset);
		buf.Push(Count);
	}

	void MaterialSlot::Serialize(u8*& ptr)
	{
		MaterialIndex = ::Read<u8>(ptr);
		Offset = ::Read<u32>(ptr);
		Count = ::Read<u32>(ptr);
	}

	void Read(const char* file, Header& header)
	{
		u8* src = ReadFile(file, true);
		u8* srcCpy = src;
		header.Serialize(srcCpy);
		delete[] src;
	}

	MeshHandle::MeshHandle(const gE::Handle<Header>& f, u8 i) : _handle(f), _mesh(&f->Meshes[i])
	{
		GE_ASSERT(i < f->MeshCount, "MESH OUT OF RANGE");
	}

	MeshHandle::MeshHandle(const gE::Handle<Header>& f, Mesh* m) : _handle(f), _mesh(m)
	{
		GE_ASSERT(m > f->Meshes && m < &f->Meshes[f->MeshCount], "MESH OUT OF RANGE")
	}

	MeshHandle::MeshHandle(const MeshHandle& o) : _handle(o._handle), _mesh(o._mesh) { }
	MeshHandle::MeshHandle(MeshHandle&& o) noexcept : _handle(o._handle), _mesh(o._mesh) { }
}
