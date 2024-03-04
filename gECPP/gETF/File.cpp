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
		Write<4>(buf, "gETF");
		Write(buf, GETF_VERSION);

		Write(buf, 0);
		Write(buf, 0);
		Write(buf, 0);
		Write(buf, 0);
		Write(buf, 0);

		Write(buf, Meshes.Count());
		for(u8 i = 0; i < Meshes.Count(); i++)
			WriteSerializable(buf, &*Meshes[i], s, 1);
	}

	void File::Serialize(istream& ptr, const File& s)
	{
		char magic[4];
		::Read<4, char>(ptr, magic);

		if (!strcmpb<4>(magic, "gETF"))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(ptr);

		if (Version >= 2) ptr.seekg(5, std::ios::cur);

		Meshes = Array<MeshReference>(::Read<u8>(ptr));
		for(u64 i = 0; i < Meshes.Count(); i++)
			Meshes[i] = gE::ref_cast(ReadNewSerializable<Mesh>(ptr, s));
	}

	MeshReference* File::GetMesh(const char* name)
	{
		for(u32 i = 0; i < Meshes.Count(); i++)
			if(!strcmp(name, Meshes[i]->Name.c_str())) return &Meshes[i];
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
