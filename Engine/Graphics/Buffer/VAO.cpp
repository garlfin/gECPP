//
// Created by scion on 9/9/2024.
//

#include "VAO.h"
#include "VAOSettings.h"

#include <Core/GUI/Editor/Editor.h>

namespace GPU
{
	void VAO::IDeserialize(istream& in, SETTINGS_T s)
	{
		Counts = Read<VAOFieldCounts>(in);
		ReadSerializable(in, Counts.MaterialCount, Materials, nullptr);
		Read(in, Counts.FieldCount, Fields);
		ReadSerializable(in, Counts.BufferCount, Buffers, s);
	}

	void VAO::ISerialize(ostream& out) const
	{
		Write(out, Counts);
		Write(out, Counts.MaterialCount, Materials);
		Write(out, Counts.FieldCount, Fields);
		Write(out, Counts.BufferCount, Buffers);
	}

	void IndexedVAO::IDeserialize(istream& in, SETTINGS_T s)
	{
		TriangleFormat = Read<GLenum>(in);
		ReadSerializable(in, TriangleBuffer, s);
	}

	void IndexedVAO::ISerialize(ostream& out) const
	{
		Write(out, TriangleFormat);
		Write(out, TriangleBuffer);
	}

	void MaterialSlot::IDeserialize(istream& in, SETTINGS_T)
	{
		Read(in, Name);
		Offset = Read<u32>(in);
		Count = Read<u32>(in);
	}

	void MaterialSlot::ISerialize(ostream& out) const
	{
		Write(out, Name);
		Write(out, Offset);
		Write(out, Count);
	}

	REFLECTABLE_ONGUI_IMPL(MaterialSlot,
	{
		DrawField(gE::Field{ "Name" }, Name, depth);
		DrawField(gE::ScalarField<u32>{ "Offset", "Tri offset" }, Offset, depth);
		DrawField(gE::ScalarField{ "Count", "Number of tris", 1u }, Count, depth);
	});

	REFLECTABLE_ONGUI_IMPL(VAO,
	{
		u8 matCount = Counts.MaterialCount;
		u8 bufCount = Counts.BufferCount;
		u8 fieldCount = Counts.FieldCount;

		DrawField(gE::ScalarField<u8>{ "Material Count", "", 1, GE_MAX_VAO_MATERIAL, 1, gE::ScalarViewMode::Input }, matCount, depth);
		DrawField(gE::ScalarField<u8>{ "Buffer Count", "", 1, GE_MAX_VAO_BUFFER, 1, gE::ScalarViewMode::Input }, bufCount, depth);
		DrawField(gE::ScalarField<u8>{ "Field Count", "", 1, GE_MAX_VAO_FIELD, 1, gE::ScalarViewMode::Input }, fieldCount, depth);

		Counts.MaterialCount = matCount;
		Counts.BufferCount = bufCount;
		Counts.FieldCount = fieldCount;

		DrawField(gE::ArrayField<gE::Field>{ "Materials" }, Materials, Counts.MaterialCount, depth);
		DrawField(gE::ArrayField<gE::Field>{ "Buffers" }, Buffers, Counts.BufferCount, depth);
	});

	REFLECTABLE_ONGUI_IMPL(IndexedVAO,
	{
		DrawField(gE::Field{ "Triangles" }, TriangleBuffer, depth);
	});

	REFLECTABLE_FACTORY_IMPL(VAO, API::VAO);
	REFLECTABLE_FACTORY_IMPL(IndexedVAO, API::IndexedVAO);
	REFLECTABLE_FACTORY_IMPL(MaterialSlot, MaterialSlot);
}
