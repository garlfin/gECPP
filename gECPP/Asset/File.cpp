//
// Created by scion on 8/13/2023.
//

#include "File.h"
#include <Engine/Utility/Binary.h>

#include "Engine/Window/Window.h"

/*
 * void Deserialize(ostream& ptr);
 * void Serialize(void*& ptr);
 *
 * 	struct Header;
 * 	struct Mesh;
 * 	struct VertexField;
 * 	struct MaterialSlot;
*/

namespace gE
{
	void Header::IDeserialize(ostream& out) const
	{
		Write(out, 4, "gETF");
		Write<u8>(out, GETF_VERSION);

		Write<u64>(out, 0);
		Write<u16>(out, 0);

		//WriteArraySerializable<u16>(buf, Meshes, *this);
	}

	void Header::ISerialize(istream& in, const Header&)
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

void IFileContainer::IDeserialize(std::ostream& out) const
{
	const TypeSystem::Type* type = _t->GetType();
	GE_ASSERT(type, "NO TYPE INFO!");

	Write(out, type->Name);
	_t->Deserialize(out);
}

void IFileContainer::ISerialize(std::istream& in, gE::Window* window)
{
	const std::string typeName = Read<std::string>(in);

	const TypeSystem::Type* type = TypeSystem::GetTypeInfo(typeName);
	GE_ASSERT(type, "NO TYPE INFO!");

	_t = type->Factory(in, window);
}