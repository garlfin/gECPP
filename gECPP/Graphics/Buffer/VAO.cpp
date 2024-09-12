//
// Created by scion on 9/9/2024.
//

#include "VAO.h"

namespace GPU
{
	void VAO::ISerialize(istream& in, SETTINGS_T s)
	{
		Counts = Read<VAOFieldCounts>(in);
		Read(in, Counts.MaterialCount, Materials);
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
}
