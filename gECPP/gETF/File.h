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
		SERIALIZABLE_PROTO;

		u8 Version = GETF_VERSION;

		Array<gE::Reference<Mesh>> Meshes;
	};

	File& Read(const char*, File&);
	NODISCARD File* Read(const char*);
}
