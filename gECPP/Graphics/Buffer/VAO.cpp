//
// Created by scion on 9/9/2024.
//

#include "VAO.h"

namespace GPU
{
	void VAO::ISerialize(istream& in, SETTINGS_T s)
	{
	}

	void VAO::IDeserialize(ostream& out) const
	{
	}

	void IndexedVAO::ISerialize(istream& in, SETTINGS_T s)
	{
		TriangleMode = Read<GLenum>(in);
		ReadSerializable(in, TriangleBuffer, s);
	}

	void IndexedVAO::IDeserialize(ostream& out) const
	{
		Write(out, TriangleMode);
		Write(out, TriangleBuffer);
	}
}
