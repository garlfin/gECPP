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
		StaticMeshEntity(gE::Window* window, const gETF::MeshHandle& mesh, const gE::Handle<gE::Material>& mat) :
			gE::Entity(window), _renderer(this, mesh, mat) {};

	 private:
		gE::MeshRenderer _renderer;
	};
}