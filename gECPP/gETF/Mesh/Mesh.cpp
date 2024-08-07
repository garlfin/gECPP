//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"

using namespace gETF;

void VertexBuffer::IDeserialize(ostream& out) const
{
	Write<u8>(out, Stride);
	Write<u32>(out, Count);
	Write(out, Stride * Count, Data.Data());
}

void VertexBuffer::ISerialize(istream& in, Mesh&)
{
	Stride = ::Read<u8>(in);
	Count = ::Read<u32>(in);

	size_t byteSize = Stride * Count;
	Data = Array<u8>(byteSize);
	Read(in, byteSize, Data.Data());
}

void VertexField::IDeserialize(ostream& out) const
{
	Write(out, Name);
	Write(out, Index);
	Write(out, BufferIndex);
	Write(out, ElementCount);
	Write(out, Offset);
	Write(out, ElementType);
	Write(out, Normalized);
}

void VertexField::ISerialize(istream& in, Mesh&)
{
	Read<char>(in, 4, Name);

	Index = ::Read<u8>(in);
	BufferIndex = ::Read<u8>(in);
	ElementCount = ::Read<u8>(in);
	Offset = ::Read<u8>(in);
	ElementType = ::Read<GLenum>(in);
	Normalized = ::Read<bool>(in);
}

void MaterialSlot::IDeserialize(ostream& out) const
{
	Write(out, Offset);
	Write(out, Count);
}

void MaterialSlot::ISerialize(istream& in, Mesh& s)
{
	Offset = ::Read<u32>(in);
	Count = ::Read<u32>(in);
}

void Mesh::IDeserialize(ostream& out) const
{
	Write(out, 4, GETF_MESH_MAGIC);
	Write(out, Version);

	WriteArray<u8>(out, Buffers);
	WriteArray<u8>(out, Fields);

	Write(out, (u8)TriMode);
	if (TriMode != TriangleMode::None) Triangles.Deserialize(out);

	WriteArray<u8>(out, Materials);
}

void Mesh::ISerialize(istream& in, gE::Window* s)
{
	char magic[4];
	Read<char>(in, 4, magic);
	Version = Read<u8>(in);

	if(!strcmpb(magic, GETF_MESH_MAGIC, 4)) std::cout << "Invalid File!\n";

	ReadArraySerializable<u8, VertexBuffer>(in, Buffers, *this);
	ReadArraySerializable<u8, VertexField>(in, Fields, *this);

	TriMode = ::Read<TriangleMode>(in);
	if (TriMode != TriangleMode::None) Triangles.Serialize(in, *this);

	ReadArraySerializable<u8, MaterialSlot>(in, Materials, *this);

	CreateVAO(s);
}

void Mesh::CreateVAO(gE::Window* w)
{
	if(VAO) return;

	if (TriMode == TriangleMode::None)
	{
		API::VAOSettings settings;
		GetVAOSettings(settings);

		VAO = gE::ptr_create<API::VAO>(w, settings);
	}
	else
	{
		API::IndexedVAOSettings settings;
		GetVAOSettings(settings);

		VAO = gE::ptr_create<API::IndexedVAO>(w, settings).Move<API::VAO>();
	}
}

void Mesh::GetVAOSettings(API::VAOSettings& settings) const
{
	GE_ASSERT(Buffers.Count() <= GE_MAX_VAO_BUFFER, "TOO MANY BUFFERS!");
	settings.BufferCount = Buffers.Count();
	for(u8 i = 0; i < settings.BufferCount; i++) settings.Buffers[i] = (API::VertexBuffer) Buffers[i];

	GE_ASSERT(Fields.Count() <= GE_MAX_VAO_FIELD, "TOO MANY FIELDS!");
	settings.FieldCount = Fields.Count();
	for(u8 i = 0; i < settings.FieldCount; i++) settings.Fields[i] = (API::VertexField) Fields[i];

	GE_ASSERT(Materials.Count() <= GE_MAX_VAO_MATERIAL, "TOO MANY MATERIALS!");
	settings.MaterialCount = Materials.Count();
	for(u8 i = 0; i < settings.MaterialCount; i++) settings.Materials[i] = (API::MaterialSlot) Materials[i];
}

void Mesh::GetVAOSettings(API::IndexedVAOSettings& settings) const
{
	GE_ASSERT(TriMode != TriangleMode::None, "CANNOT GET TRIANGLES!");

	GetVAOSettings((API::VAOSettings&) settings);
	settings.Triangles = (API::VertexField) Triangles;
}