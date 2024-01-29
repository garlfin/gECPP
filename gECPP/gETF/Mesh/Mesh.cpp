//
// Created by scion on 1/23/2024.
//

#include "Mesh.h"
#include <gETF/File.h>

using namespace gETF;

void VertexBuffer::Deserialize(SerializationBuffer& buf, const File&) const
{
	buf.Push(Stride);
	buf.Push(Count);
	//buf.PushPtr((u8*) Data, 1);
}

void VertexBuffer::Serialize(u8*& ptr, const File&)
{
	Stride = ::Read<u8>(ptr);
	Count = ::Read<u32>(ptr);

	size_t byteSize = Stride * Count;
	Data = malloc(byteSize);
	::Read(ptr, (u8*) Data, byteSize);
}

void VertexField::Deserialize(SerializationBuffer& buf, const File&) const
{
	buf.PushPrefixedString(Name);

	buf.Push(Index);
	buf.Push(BufferIndex);
	buf.Push(ElementCount);
	buf.Push(Offset);
	buf.Push(ElementType);
	buf.Push(Normalized);
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

void MaterialSlot::Deserialize(SerializationBuffer& buf, const File&) const
{
	buf.Push(Offset);
	buf.Push(Count);
}

void MaterialSlot::Serialize(u8*& ptr, const File& s)
{
	if(s.Version < 2) ::Read<u8>(ptr);
	Offset = ::Read<u32>(ptr);
	Count = ::Read<u32>(ptr);
}

void Mesh::Deserialize(SerializationBuffer& buf, const File& s) const
{
	buf.PushPtr<4>("MESH");

	buf.PushSerializableArray<u8>(Buffers, s);
	buf.PushSerializableArray<u8>(Fields, s);

	buf.Push((u8)TriangleMode);
	if (TriangleMode != TriangleMode::None) buf.PushSerializable(Triangles, s);

	buf.PushSerializableArray<u8>(Materials, s);
}

void Mesh::Serialize(u8*& ptr, const File& s)
{
	char magic[4];
	::Read<char, 4>(ptr, magic);

	if (!strcmpb<4>(magic, "MESH")) std::cout << "Invalid File!\n";
	if (s.Version >= 2) Name = ReadPrefixedString(ptr);

	Buffers = ReadArraySerializable<u8, VertexBuffer>(ptr, s);
	Fields = ReadArraySerializable<u8, VertexField>(ptr, s);

	TriangleMode = ::Read<enum TriangleMode>(ptr);
	if (TriangleMode != TriangleMode::None) Triangles.Serialize(ptr, s);

	Materials = ReadArraySerializable<u8, MaterialSlot>(ptr, s);
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
	GE_ASSERT(TriangleMode != TriangleMode::None, "CANNOT GET TRIANGLES!");

	GetVAOSettings((GL::VAOSettings&) settings);
	settings.Triangles = Triangles;
}