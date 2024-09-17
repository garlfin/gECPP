//
// Created by scion on 9/9/2024.
//

#include "VAO.h"
#include "VAOSettings.h"

namespace GPU
{
	void VAO::ISerialize(istream& in, SETTINGS_T s)
	{
		Counts = Read<VAOFieldCounts>(in);
		ReadSerializable(in, Counts.MaterialCount, Materials, nullptr);
		Read(in, Counts.FieldCount, Fields);
		ReadSerializable(in, Counts.BufferCount, Buffers, s);
	}

	void VAO::IDeserialize(ostream& out) const
	{
		Write(out, Counts);
		Write(out, Counts.MaterialCount, Materials);
		Write(out, Counts.FieldCount, Fields);
		Write(out, Counts.BufferCount, Buffers);
	}

	void IndexedVAO::ISerialize(istream& in, SETTINGS_T s)
	{
		TriangleFormat = Read<GLenum>(in);
		ReadSerializable(in, TriangleBuffer, s);
	}

	void IndexedVAO::IDeserialize(ostream& out) const
	{
		Write(out, TriangleFormat);
		Write(out, TriangleBuffer);
	}

	void MaterialSlot::ISerialize(istream& in, SETTINGS_T)
	{
		Read(in, Name);
		Offset = Read<u32>(in);
		Count = Read<u32>(in);
	}

	void MaterialSlot::IDeserialize(ostream& out) const
	{
		Write(out, Name);
		Write(out, Offset);
		Write(out, Count);
	}
}
