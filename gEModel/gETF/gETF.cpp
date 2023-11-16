//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <iostream>
#include "GL/Binary/Binary.h"
#include <GL/Buffer/VAO.h>

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
	void File::Deserialize(SerializationBuffer& buf) const
	{
		buf.PushPtr("gETF", 4);
		buf.Push<u8>(GETF_VERSION);
		buf.Push(MeshCount);
		for(u8 i = 0; i < MeshCount; i++) buf.Push(*Meshes[i].Get());
	}

	void File::Serialize(u8*& ptr)
	{
		char magic[4];
		::Read<char, 4>(ptr, magic);

		if(!strcmpb<4>(magic, "gETF"))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		u8 version = ::Read<u8>(ptr);
		MeshCount = ::Read<u8>(ptr);
		Meshes = new MeshReference[MeshCount];
		for(u8 i = 0; i < MeshCount; i++) Meshes[i] = gE::CreateReferenceFromPointer(ReadNew<Mesh>(ptr));
	}

	void Mesh::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushPtr<4>("MESH");

		buf.Push(BufferCount);
		buf.PushPtr(Buffers, BufferCount);

		buf.Push(FieldCount);
		buf.PushPtr(Fields, FieldCount);

		buf.Push(TriangleMode);
		if(TriangleMode != TriangleMode::None) buf.Push(Triangles);

		buf.Push(MaterialCount);
		buf.PushPtr(Materials, MaterialCount);
	}

	void Mesh::Serialize(u8*& ptr)
	{
		char magic[4];
		::Read<char, 4>(ptr, magic);

		if(!strcmpb<4>(magic, "MESH")) std::cout << "Invalid File!\n";

		BufferCount = ::Read<u8>(ptr);
		Buffers = new VertexBuffer[BufferCount];
		::Read(ptr, Buffers, BufferCount);

		FieldCount = ::Read<u8>(ptr);
		Fields = new VertexField[FieldCount];
		::Read(ptr, Fields, FieldCount);

		TriangleMode = ::Read<enum TriangleMode>(ptr);
		if(TriangleMode != TriangleMode::None) Triangles.Serialize(ptr);

		MaterialCount = ::Read<u8>(ptr);
		Materials = new MaterialSlot[MaterialCount];
		::Read(ptr, Materials, MaterialCount);
	}

	Mesh::~Mesh()
	{
		delete[] Name;
		delete[] Fields;
		delete[] Materials;
	}

	void Mesh::CreateVAO(gE::Window* w)
	{
		if(VAO) return;

		if(TriangleMode == TriangleMode::Simple)
			VAO = gE::SmartPointer<GL::VAO>(new GL::IndexedVAO(w, this));
		else VAO = gE::SmartPointer<GL::VAO>(new GL::VAO(w, this));
	}

	void VertexBuffer::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.Push(Index);

		buf.Push(Stride);
		buf.Push(Count);

		buf.PushPtr((u8*) Data, Stride * Count);
	}

	void VertexBuffer::Serialize(u8*& ptr)
	{
		Index = ::Read<u8>(ptr);
		Stride = ::Read<u8>(ptr);

		Count = ::Read<u32>(ptr);

		size_t byteSize = Stride * Count;
		Data = malloc(byteSize);
		::Read(ptr, (u8*) Data, byteSize);
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

	File& Read(const char* file, File& header)
	{
		u8* src = ReadFile(file, true);
		u8* srcCpy = src;
		header.Serialize(srcCpy);
		delete[] src;
		return header;
	}

	File* Read(const char* file)
	{
		auto* h = new File;
		return &Read(file, *h);
	}

	void VertexField::Serialize(u8*& ptr)
	{
		Name = ::ReadPrefixedString(ptr);

		Index = ::Read<u8>(ptr);
		BufferIndex = ::Read<u8>(ptr);
		ElementCount = ::Read<u8>(ptr);
		Offset = ::Read<u8>(ptr);
		ElementType = ::Read<GLenum>(ptr);
	}

	void VertexField::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushLengthString(Name);

		buf.Push(Index);
		buf.Push(BufferIndex);
		buf.Push(ElementCount);
		buf.Push(Offset);
		buf.Push(ElementType);
	}
}
