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

	typedef gE::Reference<Mesh> MeshReference;

	struct File : public Serializable<File>
	{
		SERIALIZABLE_PROTO;

		u8 Version = GETF_VERSION;

		Array<MeshReference> Meshes;

		MeshReference* GetMesh(const char*);
	};

	File& Read(const char*, File&);
	NODISCARD File* Read(const char*);
}
