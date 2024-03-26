//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <fstream>
#include <GL/Binary/Binary.h>

/*
 * void Deserialize(ostream& ptr);
 * void Serialize(void*& ptr);
 *
 * 	struct Header;
 * 	struct Mesh;
 * 	struct VertexField;
 * 	struct MaterialSlot;
*/

namespace gETF
{
	void File::Deserialize(ostream& buf, const File& s) const
	{
		Write(buf, "gETF", 4);
		Write<u8>(buf, GETF_VERSION);

		Write<u16>(buf, 0);
		Write<u16>(buf, 0);
		Write<u16>(buf, 0);
		Write<u16>(buf, 0);
		Write<u16>(buf, 0);

		WriteArraySerializable<u16>(buf, Meshes, *this);
	}

	void File::Serialize(istream& ptr, const File& s)
	{
		char magic[4];
		::Read<char>(ptr, magic, 4);

		if (!strcmpb(magic, "gETF", 4))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(ptr);

		if (Version >= 1) ptr.seekg(10, std::ios::cur);

		Meshes = ReadArraySerializable<u16, Mesh>(ptr, s);
	}

	Mesh* File::GetMesh(const char* name)
	{
		for(u32 i = 0; i < Meshes.Count(); i++)
			if(!strcmp(name, Meshes[i].Name.c_str())) return &Meshes[i];
		return nullptr;
	}

	File& Read(gE::Window* window, const char* file, File& header)
	{
		header.Window = window;

		std::ifstream src;
		src.open(file, std::ios::in | std::ios::binary);

		header.Serialize(src, header);

		return header;
	}

	File* Read(gE::Window* window, const char* file)
	{
		auto* h = new File();
		return &Read(window, file, *h);
	}
}
