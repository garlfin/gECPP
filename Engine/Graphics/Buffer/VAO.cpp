//
// Created by scion on 9/9/2024.
//

#include "VAO.h"

#include "VAOSettings.h"
#include "Core/GUI/Editor.inl"

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
		DrawField(gE::Field{ "Name"sv }, Name, depth);
		DrawField(gE::ScalarField<u32>{ "Offset", "Tri offset"sv }, Offset, depth);
		DrawField(gE::ScalarField{ "Count"sv, "Number of tris"sv, 1u }, Count, depth);
	});

	REFLECTABLE_ONGUI_IMPL(VAO,
	{
		u32 matCount = Counts.MaterialCount;
		u32 bufCount = Counts.BufferCount;
		u32 fieldCount = Counts.FieldCount;

		DrawField(gE::ScalarField<u32>{ "Material Count"sv, ""sv, 1, GE_MAX_VAO_MATERIAL }, matCount, depth);
		DrawField(gE::ScalarField<u32>{ "Buffer Count"sv, ""sv, 1, GE_MAX_VAO_BUFFER }, bufCount, depth);
		DrawField(gE::ScalarField<u32>{ "Field Count"sv, ""sv, 1, GE_MAX_VAO_FIELD }, fieldCount, depth);

		Counts.MaterialCount = matCount;
		Counts.BufferCount = bufCount;
		Counts.FieldCount = fieldCount;

		DrawField(gE::ArrayField<gE::Field>{ "Materials"sv }, Materials, Counts.MaterialCount, depth);
		DrawField(gE::ArrayField<gE::Field>{ "Buffers"sv }, Buffers, Counts.BufferCount, depth);
	});

	REFLECTABLE_ONGUI_IMPL(IndexedVAO,
	{
		DrawField(gE::Field{ "Triangles" }, TriangleBuffer, depth);
	});

	REFLECTABLE_FACTORY_IMPL(VAO, API::VAO);
	REFLECTABLE_FACTORY_IMPL(IndexedVAO, API::IndexedVAO);
	REFLECTABLE_FACTORY_IMPL(MaterialSlot, MaterialSlot);
}
