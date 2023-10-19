#pragma once

#include <GL/Math.h>
#include "Prototype.h"
#include <Engine/AssetManager.h>

#define GETF_VERSION 1

namespace GL { struct VAO; }

namespace gETF
{
	struct Header;
	struct Mesh;
	struct VertexField;
	struct MaterialSlot;
	struct Serializable;
	struct SerializationBuffer;

	// TODO
	// struct Scene;
	// struct Node;

	Header Import(const char*);

	struct VertexField : public Serializable
	{
		SERIALIZABLE_PROTO;

		const char* Name = nullptr;

		u8 Index = 0;

		u32 Type = 0;
		u8 TypeCount = 0;

		u32 Count = 0;

		void* Data = nullptr;

		inline void Free() { free(Data); Data = nullptr; }
		NODISCARD ALWAYS_INLINE bool IsFree() { return Data; }

		~VertexField()
		{
			free(Data);
			delete[] Name;
		}
	};

	struct MaterialSlot : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialIndex;
		u32 Offset;
		u32 Count;
	};

	enum class TriangleMode : u8
	{
		None,
		Simple,
		Complex
	};

	struct Mesh : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialCount;
		u8 FieldCount;
		TriangleMode TriangleMode;

		VertexField Triangles;
		VertexField* Fields;
		MaterialSlot* Materials;

		void Free() { Triangles.Free(); for(u8 i = 0; i < FieldCount; i++) Fields[i].Free(); }

		~Mesh()
		{
			delete[] Fields;
			delete[] Materials;
		}
	};

	struct Header : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MeshCount = 0;
		Mesh* Meshes = nullptr;

		~Header()
		{
			delete[] Meshes;
		}
	};

	class MeshHandle
	{
	 public:
		MeshHandle(const gE::Handle<Header>&, u8);
		MeshHandle(const gE::Handle<Header>&, Mesh*);

		MeshHandle(const MeshHandle&);
		MeshHandle(MeshHandle&&) noexcept;

		OPERATOR_EQUALS_BOTH(MeshHandle);

		GET_CONST_VALUE(Header*, File, _handle.Get());
		GET_CONST_VALUE(Mesh*,, _mesh);

		ALWAYS_INLINE Mesh* operator->() const { return Get(); }

	 private:
		gE::Handle<Header> _handle;
	 	Mesh* _mesh;
	};

	void Read(const char*, Header&);
}
