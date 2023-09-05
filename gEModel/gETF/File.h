#pragma once

#include <GL/GLMath.h>
#include "Prototype.h"

#define GETF_VERSION 1

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

		~VertexField()
		{
			delete[] (u8*)Data;
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

		~Mesh()
		{
			delete[] Fields;
			delete[] Materials;
		}
	};

	struct Header : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MeshCount;
		Mesh* Meshes;

		~Header()
		{
			delete[] Meshes;
		}
	};

	void Read(const char*, Header&);
}
