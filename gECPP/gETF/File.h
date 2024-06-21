#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>
#include <Engine/AssetManager.h>
#include "Mesh/Mesh.h"

#define GETF_VERSION 1

namespace gETF
{
	struct Header : public Serializable<Header>
	{
	 public:
		SERIALIZABLE_PROTO_T(Header, Serializable<Header>);

		gE::Window* Window;
		u8 Version = GETF_VERSION;

	 private:

		Array<u8> Files;
	};
  }
