//
// Created by scion on 8/10/2023.
//

#pragma once

#include <GLMath.h>

#define TRIANGLE_MODE_SIMPLE 0
#define TRIANGLE_MODE_COMPLEX 1

namespace gETF
{
	struct Header;
	struct Mesh;
	struct VertexField;
	struct MaterialSlot;

	// TODO
	// struct Scene;
	// struct Node;

	Header Import(const char*);
}
struct gETF::VertexField
{
	const char* Name;

	u8 Index;

	u32 Type;
	u8 TypeCount;

	u32 Count;

	void* Data;
};

struct gETF::MaterialSlot
{
	u8 MaterialIndex;
	u32 Offset;
	u32 Count;
};

struct gETF::Mesh
{
	u8 MaterialCount;
	u8 FieldCount;
	u8 TriangleMode;

	VertexField* Fields;
	MaterialSlot* Materials;
};

struct gETF::Header
{
	u8 MeshCount;
	Mesh* Meshes;
};


