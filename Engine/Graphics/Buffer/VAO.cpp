//
// Created by scion on 9/9/2024.
//

#include "VAO.h"
#include "VAOSettings.h"

#include <Core/GUI/Editor/Editor.h>

namespace GPU
{
	GLenum ToGLEnum(PrimitiveType primType)
	{
		switch(primType)
		{
		case PrimitiveType::Point: return GL_POINTS;
		case PrimitiveType::Line: return GL_LINES;
		case PrimitiveType::Triangle: return GL_TRIANGLES;
		default:
			assert(false);
		}
		return GL_NONE;
	}

	REFLECTABLE_CLASS(VAO, {
		REFLECT_MEMBER(ScalarField<u32>, Test, FieldFlags::Default, "Tip")
	}, 1);

	void VAO::IDeserialize(istream& in, SETTINGS_T s)
	{
		Counts = Read<VAOFieldCounts>(in);
		ReadSerializable(in, Counts.MaterialCount, Materials, nullptr);
		ReadSerializable(in, Counts.FieldCount, Fields, nullptr);
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
		IndicesFormat = Read<GLenum>(in);
		ReadSerializable(in, IndicesBuffer, s);
	}

	void IndexedVAO::ISerialize(ostream& out) const
	{
		Write(out, IndicesFormat);
		Write(out, IndicesBuffer);
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

	void VertexField::IDeserialize(istream& in, SETTINGS_T s)
	{
		static_assert(sizebetween(VertexField, Name, Offset) == 12);
		Read<char>(in, 12, Name);
	}

	void VertexField::ISerialize(ostream& out) const
	{
		Write<char>(out, 12, Name);
	}

	REFLECTABLE_ONGUI_IMPL(MaterialSlot,
	{
		/*DrawField(gE::Field{ "Name" }, Name, depth);
		DrawField(gE::ScalarField<u32>{ "Offset", "Tri offset" }, Offset, depth);
		DrawField(gE::ScalarField{ "Count", "Number of tris", 1u }, Count, depth);*/
	});
	REFLECTABLE_NAME_IMPL(MaterialSlot, return Name);
	REFLECTABLE_FACTORY_IMPL(MaterialSlot);

	REFLECTABLE_ONGUI_IMPL(VAO,
	{
		/*u8 matCount = Counts.MaterialCount;
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
		DrawField(gE::ArrayField<gE::Field>{ "Fields" }, Fields, Counts.FieldCount, depth);*/
	});
	API_REFLECTABLE_FACTORY_IMPL(VAO, API::VAO);

	REFLECTABLE_ONGUI_IMPL(IndexedVAO,
	{
		//DrawField(gE::Field{ "Triangles" }, IndicesBuffer, depth);
	});
	API_REFLECTABLE_FACTORY_IMPL(IndexedVAO, API::IndexedVAO);

	REFLECTABLE_ONGUI_IMPL(VertexField,
	{
		/*DrawField<const std::span<char>>(gE::Field{ "Name" }, std::span(Name, Name + 4), depth);
		DrawField(gE::EnumField{ "Element Type", "", EElementType}, ElementType, depth);

		bool normalized = Normalized;
		DrawField(gE::Field{ "Normalized" }, normalized, depth);
		Normalized = normalized;

		u8 bufIndex = BufferIndex;
		DrawField(gE::ScalarField<u8>{ "Buffer Index", "", 0, GE_MAX_VAO_BUFFER}, bufIndex, depth);
		BufferIndex = bufIndex;

		DrawField(gE::ScalarField<u8>{ "Index", "", 0, GE_MAX_VAO_FIELD}, Index, depth);
		DrawField(gE::ScalarField<u8>{ "Element Count", "", 1, 4}, ElementCount, depth);
		DrawField(gE::ScalarField<u8>{ "Offset" }, Offset, depth);*/
	});
	REFLECTABLE_NAME_IMPL(VertexField, return std::string(Name, 4));
	REFLECTABLE_FACTORY_IMPL(VertexField);
}
