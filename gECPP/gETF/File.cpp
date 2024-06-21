//
// Created by scion on 8/13/2023.
//

#include "File.h"
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
	void Header::IDeserialize(ostream& buf) const
	{
		Write(buf, "gETF", 4);
		Write<u8>(buf, GETF_VERSION);

		Write<u64>(buf, 0);
		Write<u16>(buf, 0);

		//WriteArraySerializable<u16>(buf, Meshes, *this);
	}

	void Header::ISerialize(istream& ptr, const Header& s)
	{
		char magic[4];
		Read<char>(ptr, magic, 4);

		if (!strcmpb(magic, "gETF", 4))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(ptr);

		if (Version >= 1) ptr.seekg(10, std::ios::cur);

		//Meshes = ReadArraySerializable<u16, Mesh>(ptr, s);
	}

	void IFileContainer::IDeserialize(std::ostream& buf) const
	{
		Write(buf, _type);
		WritePrefixedString(buf, _name);
		_t->Deserialize(buf);
	}

	void IFileContainer::ISerialize(std::istream& ptr, gE::Window* const& settings)
	{
		_type = Read<TypeID>(ptr);
		_name = ReadPrefixedString(ptr);
		_t->Serialize(ptr, settings);
	}
}
