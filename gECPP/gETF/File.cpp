//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <Engine/Binary/Binary.h>

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
		Write(buf, 4, "gETF");
		Write<u8>(buf, GETF_VERSION);

		Write<u64>(buf, 0);
		Write<u16>(buf, 0);

		//WriteArraySerializable<u16>(buf, Meshes, *this);
	}

	void Header::ISerialize(istream& ptr, const Header& s)
	{
		char magic[4];
		Read<char>(ptr, 4, magic);

		if (!strcmpb(magic, "gETF", 4))
		{
			std::cout << "Invalid File!" << std::endl;
			return;
		}

		Version = ::Read<u8>(ptr);

		if (Version >= 1) ptr.seekg(10, std::ios::cur);

		//Meshes = ReadArraySerializable<u16, Mesh>(ptr, s);
	}
}

void IFileContainer::IDeserialize(std::ostream& buf) const
{
	Write(buf, _type);
	Write(buf, _name);
	_t->Deserialize(buf);
}

void IFileContainer::ISerialize(std::istream& ptr, gE::Window* settings)
{
	_type = Read<TypeSystem<gE::Window*>::TypeID>(ptr);
	_name = Read<std::string>(ptr);
	//_t->Serialize(ptr, settings);
}