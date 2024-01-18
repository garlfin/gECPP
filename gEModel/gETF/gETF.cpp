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
	void File::Deserialize(SerializationBuffer& buf, const File& s) const
	{
		buf.PushPtr<4>("gETF");
		buf.Push<u8>(GETF_VERSION);
		buf.Push(MeshCount);
		for(u8 i = 0; i < MeshCount; i++) buf.PushSerializable(*Meshes[i].Get(), s);
	}

	void File::Serialize(u8*& ptr, const File& s)
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
		for(u8 i = 0; i < MeshCount; i++) Meshes[i] = gE::ref_cast(ReadNewSerializable<Mesh>(ptr, s));
	}

	void Mesh::Deserialize(gETF::SerializationBuffer& buf, const File& s) const
	{
		buf.PushPtr<4>("MESH");

		buf.Push(BufferCount);
		buf.PushSerializablePtr(Buffers, s, BufferCount);

		buf.Push(FieldCount);
		buf.PushSerializablePtr(Fields, s, FieldCount);

		buf.Push(TriangleMode);
		if(TriangleMode != TriangleMode::None) buf.PushSerializable(Triangles, s);

		buf.Push(MaterialCount);
		buf.PushSerializablePtr(Materials, s, MaterialCount);
	}

	void Mesh::Serialize(u8*& ptr, const File& s)
	{
		char magic[4];
		::Read<char, 4>(ptr, magic);

		if(!strcmpb<4>(magic, "MESH")) std::cout << "Invalid File!\n";

		BufferCount = ::Read<u8>(ptr);
		Buffers = new VertexBuffer[BufferCount];
		ReadSerializable(ptr, Buffers, s, BufferCount);

		FieldCount = ::Read<u8>(ptr);
		Fields = new VertexField[FieldCount];
		ReadSerializable(ptr, Fields, s, FieldCount);

		TriangleMode = ::Read<enum TriangleMode>(ptr);
		if(TriangleMode != TriangleMode::None) Triangles.Serialize(ptr, s);

		MaterialCount = ::Read<u8>(ptr);
		Materials = new MaterialSlot[MaterialCount];
		ReadSerializable(ptr, Materials, s, MaterialCount);
	}

	Mesh::~Mesh()
	{
		delete[] Fields;
		delete[] Materials;
	}

	void Mesh::CreateVAO(gE::Window* w)
	{
		if(VAO) return;

		GL::IndexedVAOSettings settings;
		GetVAOSettings(settings);

		if (TriangleMode == TriangleMode::None)
			VAO = gE::SmartPointer<GL::VAO>(new GL::VAO(w, settings));
		else
			VAO = gE::SmartPointer<GL::VAO>(new GL::IndexedVAO(w, settings));
	}

	void Mesh::GetVAOSettings(GL::VAOSettings& settings) const
	{
		GE_ASSERT(BufferCount <= GE_MAX_VAO_BUFFER, "TOO MANY BUFFERS!");
		settings.BufferCount = BufferCount;
		for(u8 i = 0; i < BufferCount; i++) settings.Buffers[i] = Buffers[i];

		GE_ASSERT(FieldCount <= GE_MAX_VAO_FIELD, "TOO MANY FIELDS!");
		settings.FieldCount = FieldCount;
		for(u8 i = 0; i < FieldCount; i++) settings.Fields[i] = Fields[i];

		GE_ASSERT(MaterialCount <= GE_MAX_VAO_MATERIAL, "TOO MANY MATERIALS!");
		settings.MaterialCount = MaterialCount;
		for(u8 i = 0; i < MaterialCount; i++) settings.Materials[i] = Materials[i];
	}

	void Mesh::GetVAOSettings(GL::IndexedVAOSettings& settings) const
	{
		GE_ASSERT(TriangleMode != TriangleMode::None, "CANNOT GET TRIANGLES!");

		GetVAOSettings((GL::VAOSettings&) settings);
		settings.Triangles = Triangles;
	}

	void VertexBuffer::Deserialize(gETF::SerializationBuffer& buf, const File&) const
	{
		buf.Push(Stride);
		buf.Push(Count);
		buf.PushPtr((u8*) Data, Stride * Count);
	}

	void VertexBuffer::Serialize(u8*& ptr, const File&)
	{
		Stride = ::Read<u8>(ptr);
		Count = ::Read<u32>(ptr);

		size_t byteSize = Stride * Count;
		Data = malloc(byteSize);
		::Read(ptr, (u8*) Data, byteSize);
	}

	void MaterialSlot::Deserialize(gETF::SerializationBuffer& buf, const File&) const
	{
		buf.Push(MaterialIndex);
		buf.Push(Offset);
		buf.Push(Count);
	}

	void MaterialSlot::Serialize(u8*& ptr, const File&)
	{
		MaterialIndex = ::Read<u8>(ptr);
		Offset = ::Read<u32>(ptr);
		Count = ::Read<u32>(ptr);
	}

	File& Read(const char* file, File& header)
	{
		u8* src = ReadFile(file, true);
		u8* srcCpy = src;
		header.Serialize(srcCpy, header);
		delete[] src;
		return header;
	}

	File* Read(const char* file)
	{
		auto* h = new File;
		return &Read(file, *h);
	}

	void VertexField::Serialize(u8*& ptr, const File&)
	{
		::Read<char, 4>(ptr, (char*) Name);

		Index = ::Read<u8>(ptr);
		BufferIndex = ::Read<u8>(ptr);
		ElementCount = ::Read<u8>(ptr);
		Offset = ::Read<u8>(ptr);
		ElementType = ::Read<GLenum>(ptr);
		Normalized = ::Read<bool>(ptr);
	}

	void VertexField::Deserialize(gETF::SerializationBuffer& buf, const File&) const
	{
		buf.PushPrefixedString(Name);

		buf.Push(Index);
		buf.Push(BufferIndex);
		buf.Push(ElementCount);
		buf.Push(Offset);
		buf.Push(ElementType);
		buf.Push(Normalized);
	}
}
