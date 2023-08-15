//
// Created by scion on 8/13/2023.
//

#include "gETF.h"

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
	void Header::Deserialize(SerializationBuffer& buf) const
	{
		buf.PushPtr("gETF", 4);
		buf.Push<u8>(GETF_VERSION);
		buf.Push(MeshCount);
		buf.PushPtr(Meshes, MeshCount);
	}

	void Header::Serialize(void*& ptr) const {}

	void Mesh::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushPtr("MESH", 4);
		buf.Push(FieldCount);
		buf.PushPtr(Fields, FieldCount);
		buf.Push(TriangleMode);
		buf.Push(MaterialCount);
		buf.PushPtr(Materials, MaterialCount);
	}

	void Mesh::Serialize(void*& ptr) const {}

	void VertexField::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.PushString(Name);
		buf.Push(Index);
		buf.Push(Type);
		buf.Push(TypeCount);
		buf.Push(Count);
		buf.PushPtr((u8*) Data, Count * TypeCount * GetSizeOfGLType(Type));
	}

	void VertexField::Serialize(void*& ptr) const { }

	void MaterialSlot::Deserialize(gETF::SerializationBuffer& buf) const
	{
		buf.Push(MaterialIndex);
		buf.Push(Offset);
		buf.Push(Count);
	}

	void MaterialSlot::Serialize(void*& ptr) const
	{

	}

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count)
	{
		for (u32 i = 0; i < count; i++) PushPtr(t[i].Data(), t[i].Length());
	}
}

