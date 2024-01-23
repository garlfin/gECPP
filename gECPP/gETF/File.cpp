//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <iostream>
#include "GL/Binary/Binary.h"
#include "GL/Buffer/VAO.h"

/*
 * void Deserialize(SerializationBuffer& ptr);
 * void Serialize(void*& ptr);
 *
 * 	struct Header;
 * 	struct Mesh;
 * 	struct VertexField;
 * 	struct MaterialSlot;
*/

namespace gETF
{
	void File::Deserialize(SerializationBuffer& buf, const File& s) const
	{
		buf.PushPtr<4>("gETF");
		buf.Push<u8>(GETF_VERSION);

		buf.Push(0);
		buf.Push(0);
		buf.Push(0);
		buf.Push(0);
		buf.Push(0);

		buf.Push(Meshes.Count());
		for(u8 i = 0; i < Meshes.Count(); i++) buf.PushSerializable(*Meshes[i].Get(), s);
	}

	void File::Serialize(u8*& ptr, const File& s)
	{
		char magic[4];
		::Read<char, 4>(ptr, magic);

		if (!strcmpb<4>(magic, "gETF"))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(ptr);

		if (Version >= 2)
		{
			::Read<u8>(ptr);
			::Read<u8>(ptr);
			::Read<u8>(ptr);
			::Read<u8>(ptr);
			::Read<u8>(ptr);
		}

		Meshes = Array<gE::Reference<Mesh>>(::Read<u8>(ptr));
		for(u8 i = 0; i < Meshes.Count(); i++) Meshes[i] = gE::ref_cast(ReadNewSerializable<Mesh>(ptr, s));
	}

	File& Read(const char* file, File& header)
	{
		u8* src = ReadFile(file, true);
		u8* srcCpy = src;
		header.Serialize(srcCpy, header);
		delete[] src;
		return header;
	}

	File* Read(const char* file)
	{
		auto* h = new File;
		return &Read(file, *h);
	}
}
