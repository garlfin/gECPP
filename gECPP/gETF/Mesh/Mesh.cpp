//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"

using namespace gETF;

void VertexBuffer::IDeserialize(ostream& buf) const
{
	Write<u8>(buf, Stride);
	Write<u32>(buf, Count);
	Write(buf, Data.Data(), Stride * Count);
}

void VertexBuffer::ISerialize(istream& ptr, const Mesh&)
{
	Stride = ::Read<u8>(ptr);
	Count = ::Read<u32>(ptr);

	size_t byteSize = Stride * Count;
	Data = Array<u8>(byteSize);
	Read(ptr, Data.Data(), byteSize);
}

void VertexField::IDeserialize(ostream& buf) const
{
	WritePrefixedString(buf, Name);
	Write(buf, Index);
	Write(buf, BufferIndex);
	Write(buf, ElementCount);
	Write(buf, Offset);
	Write(buf, ElementType);
	Write(buf, Normalized);
}

void VertexField::ISerialize(istream& ptr, const Mesh&)
{
	Read<char>(ptr, Name, 4);

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

void MaterialSlot::ISerialize(istream& ptr, const Mesh& s)
{
	Offset = ::Read<u32>(ptr);
	Count = ::Read<u32>(ptr);
}

void Mesh::IDeserialize(ostream& buf) const
{
	Write(buf, GETF_MESH_MAGIC, 4);
	Write(buf, Version);

	WriteArraySerializable<u8>(buf, Buffers);
	WriteArraySerializable<u8>(buf, Fields);

	Write(buf, (u8)TriMode);
	if (TriMode != TriangleMode::None) Triangles.Deserialize(buf);

	WriteArraySerializable<u8>(buf, Materials);
}

void Mesh::ISerialize(istream& ptr, gE::Window* const& s)
{
	char magic[4];
	Read<char>(ptr, magic, 4);
	Version = Read<u8>(ptr);

	if(!strcmpb(magic, GETF_MESH_MAGIC, 4)) std::cout << "Invalid File!\n";

	ReadArraySerializable<u8, VertexBuffer>(Buffers, ptr, *this);
	ReadArraySerializable<u8, VertexField>(Fields, ptr, *this);

	TriMode = ::Read<TriangleMode>(ptr);
	if (TriMode != TriangleMode::None) Triangles.Serialize(ptr, *this);

	ReadArraySerializable<u8, MaterialSlot>(Materials, ptr, *this);

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