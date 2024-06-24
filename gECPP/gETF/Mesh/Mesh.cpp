//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"

using namespace gETF;

void VertexBuffer::IDeserialize(ostream& buf) const
{
	Write<u8>(buf, Stride);
	Write<u32>(buf, Count);
	Write(buf, Stride * Count, Data.Data());
}

void VertexBuffer::ISerialize(istream& ptr, Mesh&)
{
	Stride = ::Read<u8>(ptr);
	Count = ::Read<u32>(ptr);

	size_t byteSize = Stride * Count;
	Data = Array<u8>(byteSize);
	Read(ptr, byteSize, Data.Data());
}

void VertexField::IDeserialize(ostream& buf) const
{
	Write(buf, Name);
	Write(buf, Index);
	Write(buf, BufferIndex);
	Write(buf, ElementCount);
	Write(buf, Offset);
	Write(buf, ElementType);
	Write(buf, Normalized);
}

void VertexField::ISerialize(istream& ptr, Mesh&)
{
	Read<char>(ptr, 4, Name);

	Index = ::Read<u8>(ptr);
	BufferIndex = ::Read<u8>(ptr);
	ElementCount = ::Read<u8>(ptr);
	Offset = ::Read<u8>(ptr);
	ElementType = ::Read<GLenum>(ptr);
	Normalized = ::Read<bool>(ptr);
}

void MaterialSlot::IDeserialize(ostream& buf) const
{
	Write(buf, Offset);
	Write(buf, Count);
}

void MaterialSlot::ISerialize(istream& ptr, Mesh& s)
{
	Offset = ::Read<u32>(ptr);
	Count = ::Read<u32>(ptr);
}

void Mesh::IDeserialize(ostream& buf) const
{
	Write(buf, 4, GETF_MESH_MAGIC);
	Write(buf, Version);

	WriteArray<u8>(buf, Buffers);
	WriteArray<u8>(buf, Fields);

	Write(buf, (u8)TriMode);
	if (TriMode != TriangleMode::None) Triangles.Deserialize(buf);

	WriteArray<u8>(buf, Materials);
}

void Mesh::ISerialize(istream& ptr, gE::Window* s)
{
	char magic[4];
	Read<char>(ptr, 4, magic);
	Version = Read<u8>(ptr);

	if(!strcmpb(magic, GETF_MESH_MAGIC, 4)) std::cout << "Invalid File!\n";

	ReadArraySerializable<u8, VertexBuffer>(ptr, Buffers, *this);
	ReadArraySerializable<u8, VertexField>(ptr, Fields, *this);

	TriMode = ::Read<TriangleMode>(ptr);
	if (TriMode != TriangleMode::None) Triangles.Serialize(ptr, *this);

	ReadArraySerializable<u8, MaterialSlot>(ptr, Materials, *this);

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
	for(u8 i = 0; i < settings.BufferCount; i++) settings.Buffers[i] = (GL::VertexBuffer) Buffers[i];

	GE_ASSERT(Fields.Count() <= GE_MAX_VAO_FIELD, "TOO MANY FIELDS!");
	settings.FieldCount = Fields.Count();
	for(u8 i = 0; i < settings.FieldCount; i++) settings.Fields[i] = (GL::VertexField) Fields[i];

	GE_ASSERT(Materials.Count() <= GE_MAX_VAO_MATERIAL, "TOO MANY MATERIALS!");
	settings.MaterialCount = Materials.Count();
	for(u8 i = 0; i < settings.MaterialCount; i++) settings.Materials[i] = (GL::MaterialSlot) Materials[i];
}

void Mesh::GetVAOSettings(GL::IndexedVAOSettings& settings) const
{
	GE_ASSERT(TriMode != TriangleMode::None, "CANNOT GET TRIANGLES!");

	GetVAOSettings((GL::VAOSettings&) settings);
	settings.Triangles = (GL::VertexField) Triangles;
}