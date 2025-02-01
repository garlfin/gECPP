//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <Engine/Utility/Binary.h>

#include "Engine/Window/Window.h"

/*
 * void Serialize(ostream& ptr);
 * void Deserialize(void*& ptr);
 *
 * 	struct Header;
 * 	struct Mesh;
 * 	struct VertexField;
 * 	struct MaterialSlot;
*/

namespace gE
{
	void Header::ISerialize(ostream& out) const
	{
		Write(out, 4, "gETF");
		Write<u8>(out, GETF_VERSION);

		Write<u64>(out, 0);
		Write<u16>(out, 0);

		//WriteArraySerializable<u16>(buf, Meshes, *this);
	}

	void Header::IDeserialize(istream& in, const Header&)
	{
		char magic[4];
		Read<char>(in, 4, magic);

		if (!strcmpb(magic, "gETF", 4))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(in);

		if (Version >= 1) in.seekg(10, std::ios::cur);

		//Meshes = ReadArraySerializable<u16, Mesh>(ptr, s);
	}
}