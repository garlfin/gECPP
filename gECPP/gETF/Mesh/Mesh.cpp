//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"
#include <gETF/File.h>

using namespace gETF;

void VertexBuffer::Deserialize(ostream& buf) const
{
	Write<u8>(buf, Stride);
	Write<u32>(buf, Count);
	Write(buf, (u8*) Data, Stride * Count);
}

void VertexBuffer::Serialize(istream& ptr, const Mesh&)
{
	Stride = ::Read<u8>(ptr);
	Count = ::Read<u32>(ptr);

	size_t byteSize = Stride * Count;
	Data = malloc(byteSize);
	Read(ptr, (u8*) Data, byteSize);
}

void VertexField::Deserialize(ostream& buf) const
{
	WritePrefixedString(buf, Name);
	Write(buf, Index);
	Write(buf, BufferIndex);
	Write(buf, ElementCount);
	Write(buf, Offset);
	Write(buf, ElementType);
	Write(buf, Normalized);
}

void VertexField::Serialize(istream& ptr, const Mesh&)
{
	Read<char>(ptr, Name, 4);

	Index = ::Read<u8>(ptr);
	BufferIndex = ::Read<u8>(ptr);
	ElementCount = ::Read<u8>(ptr);
	Offset = ::Read<u8>(ptr);
	ElementType = ::Read<GLenum>(ptr);
	Normalized = ::Read<bool>(ptr);
}

void MaterialSlot::Deserialize(ostream& buf) const
{
	Write(buf, Offset);
	Write(buf, Count);
}

void MaterialSlot::Serialize(istream& ptr, const Mesh& s)
{
	Offset = ::Read<u32>(ptr);
	Count = ::Read<u32>(ptr);
}

void Mesh::Deserialize(ostream& buf) const
{
	Write(buf, GETF_MESH_MAGIC, 4);
	Write(buf, Version);

	WriteArraySerializable<u8>(buf, Buffers);
	WriteArraySerializable<u8>(buf, Fields);

	Write(buf, (u8)TriMode);
	if (TriMode != TriangleMode::None) Triangles.Deserialize(buf);

	WriteArraySerializable<u8>(buf, Materials);
}

void Mesh::Serialize(istream& ptr, gE::Window* const& s)
{
	char magic[4];
	Read<char>(ptr, magic, 4);
	Version = Read<u8>(ptr);

	if(!strcmpb(magic, GETF_MESH_MAGIC, 4)) std::cout << "Invalid File!\n";

	Buffers = ReadArraySerializable<u8, VertexBuffer>(ptr, *this);
	Fields = ReadArraySerializable<u8, VertexField>(ptr, *this);

	TriMode = ::Read<TriangleMode>(ptr);
	if (TriMode != TriangleMode::None) Triangles.Serialize(ptr, *this);

	Materials = ReadArraySerializable<u8, MaterialSlot>(ptr, *this);

	CreateVAO(s);
}

void Mesh::CreateVAO(gE::Window* w)
{
	if(VAO) return;

	if (TriMode == TriangleMode::None)
	{
		GL::VAOSettings settings;
		GetVAOSettings(settings);

		VAO = gE::ptr_create<GL::VAO>(w, settings);
	}
	else
	{
		GL::IndexedVAOSettings settings;
		GetVAOSettings(settings);

		VAO = gE::ptr_create<GL::IndexedVAO>(w, settings).Move<GL::VAO>();
	}
}

void Mesh::GetVAOSettings(GL::VAOSettings& settings) const
{
	GE_ASSERT(Buffers.Count() <= GE_MAX_VAO_BUFFER, "TOO MANY BUFFERS!");
	settings.BufferCount = Buffers.Count();
	for(u8 i = 0; i < settings.BufferCount; i++) settings.Buffers[i] = Buffers[i];

	GE_ASSERT(Fields.Count() <= GE_MAX_VAO_FIELD, "TOO MANY FIELDS!");
	settings.FieldCount = Fields.Count();
	for(u8 i = 0; i < settings.FieldCount; i++) settings.Fields[i] = Fields[i];

	GE_ASSERT(Materials.Count() <= GE_MAX_VAO_MATERIAL, "TOO MANY MATERIALS!");
	settings.MaterialCount = Materials.Count();
	for(u8 i = 0; i < settings.MaterialCount; i++) settings.Materials[i] = Materials[i];
}

void Mesh::GetVAOSettings(GL::IndexedVAOSettings& settings) const
{
	GE_ASSERT(TriMode != TriangleMode::None, "CANNOT GET TRIANGLES!");

	GetVAOSettings((GL::VAOSettings&) settings);
	settings.Triangles = Triangles;
}