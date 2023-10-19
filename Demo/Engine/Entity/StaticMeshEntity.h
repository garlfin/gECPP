//
// Created by scion on 10/6/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "gEModel/gETF.h"
#include "Engine/Component/MeshRenderer.h"

namespace VoxelDemo
{
 	class StaticMeshEntity : public gE::Entity
	{
	 public:
		StaticMeshEntity(gE::Window* window, gETF::MeshHandle& mesh) :
			gE::Entity(window), _renderer(this, mesh) {};

	 private:
		gE::MeshRenderer _renderer;
	};
}