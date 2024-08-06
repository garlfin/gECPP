//
// Created by scion on 10/6/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Component/MeshRenderer.h"
#include "Engine/Component/MaterialHolder.h"

namespace gE::VoxelDemo
{
 	class StaticMeshEntity : public Entity
	{
	 public:
		StaticMeshEntity(Window* window, gETF::Mesh* mesh, const Array<Reference<Material>>& mat = {}) :
			Entity(window),
			_renderer(this, mesh, _materialHolder),
			_materialHolder(this, mat)
		{ };

		GET(gE::MeshRenderer&, Renderer, _renderer);
		GET(gE::MaterialHolder&, Materials, _materialHolder);

	 private:
		MeshRenderer _renderer;
		MaterialHolder _materialHolder;
	};
}