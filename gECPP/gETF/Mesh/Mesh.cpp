//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"
#include <gETF/File.h>

using namespace gETF;

void VertexBuffer::Deserialize(ostream& buf, const File&) const
{
	Write(buf, Stride);
	Write(buf, Count);
	Write(buf, (u8*) Data, Stride * Count);
}

void VertexBuffer::Serialize(istream& ptr, const File&)
{
	Stride = ::Read<u8>(ptr);
	Count = ::Read<u32>(ptr);

	size_t byteSize = Stride * Count;
	Data = malloc(byteSize);
	::Read(ptr, (u8*) Data, byteSize);
}

void VertexField::Deserialize(ostream& buf, const File&) const
{
	WritePrefixedString(buf, Name);

	Write(buf, Index);
	Write(buf, BufferIndex);
	Write(buf, ElementCount);
	Write(buf, Offset);
	Write(buf, ElementType);
	Write(buf, Normalized);
}

void VertexField::Serialize(istream& ptr, const File&)
{
	::Read<char>(ptr, Name, 4);

	Index = ::Read<u8>(ptr);
	BufferIndex = ::Read<u8>(ptr);
	ElementCount = ::Read<u8>(ptr);
	Offset = ::Read<u8>(ptr);
	ElementType = ::Read<GLenum>(ptr);
	Normalized = ::Read<bool>(ptr);
}

void MaterialSlot::Deserialize(ostream& buf, const File&) const
{
	Write(buf, Offset);
	Write(buf, Count);
}

void MaterialSlot::Serialize(istream& ptr, const File& s)
{
	if(s.Version < 2) ::Read<u8>(ptr);
	Offset = ::Read<u32>(ptr);
	Count = ::Read<u32>(ptr);
}

void Mesh::Deserialize(ostream& buf, const File& s) const
{
	Write(buf, "MESH", 4);

	WriteArraySerializable<u8>(buf, Buffers, s);
	WriteArraySerializable<u8>(buf, Fields, s);

	Write(buf, (u8)TriMode);
	if (TriMode != TriangleMode::None) Triangles.Deserialize(buf, s);

	WriteArraySerializable<u8>(buf, Materials, s);
}

void Mesh::Serialize(istream& ptr, const File& s)
{
	char magic[4];
	::Read<char>(ptr, magic, 4);

	if(!strcmpb(magic, "MESH", 4)) std::cout << "Invalid File!\n";
	if(s.Version >= 2) Name = ReadPrefixedString(ptr);

	Buffers = ReadArraySerializable<u8, VertexBuffer>(ptr, s);
	Fields = ReadArraySerializable<u8, VertexField>(ptr, s);

	TriMode = ::Read<TriangleMode>(ptr);
	if (TriMode != TriangleMode::None) Triangles.Serialize(ptr, s);

	Materials = ReadArraySerializable<u8, MaterialSlot>(ptr, s);
}

void Mesh::CreateVAO(gE::Window* w)
{
	if(VAO) return;

	GL::IndexedVAOSettings settings;
	GetVAOSettings(settings);

	if (TriMode == TriangleMode::None)
		VAO = gE::SmartPointer<GL::VAO>(new GL::VAO(w, settings));
	else
		VAO = gE::SmartPointer<GL::VAO>(new GL::IndexedVAO(w, settings));
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