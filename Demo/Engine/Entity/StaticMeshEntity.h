//
// Created by scion on 10/6/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Component/MeshRenderer.h"
#include "Engine/Component/MaterialHolder.h"

namespace VoxelDemo
{
 	class StaticMeshEntity : public gE::Entity
	{
	 public:
		StaticMeshEntity(gE::Window* window, gETF::Mesh* mesh, const Array<gE::Reference<gE::Material>>& mat = {}) :
			gE::Entity(window),
			_renderer(this, mesh, _materialHolder),
			_materialHolder(this, mat)
		{ };

		GET(gE::MeshRenderer&, Renderer, _renderer);
		GET(gE::MaterialHolder&, Materials, _materialHolder);

	 private:
		gE::MeshRenderer _renderer;
		gE::MaterialHolder _materialHolder;
	};
}