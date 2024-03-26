#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>
#include <Engine/AssetManager.h>
#include "Mesh/Mesh.h"

#define GETF_VERSION 1

namespace gETF
{
	// TODO
	// struct Scene;
	// struct Node;

	struct File : public Serializable<File>
	{
		SERIALIZABLE_PROTO_T;

		gE::Window* Window;
		u8 Version = GETF_VERSION;

		Array<Mesh> Meshes;

		Mesh* GetMesh(const char*);
	};

	File& Read(gE::Window*, const char*, File&);
	NODISCARD File* Read(gE::Window*, const char*);
}
