//
// Created by scion on 10/6/2023.
//

#pragma once

#include <Engine/Component/MaterialHolder.h>
#include <Engine/Component/MeshRenderer.h>
#include <Engine/Entity/Entity.h>

namespace gE::VoxelDemo
{
 	class StaticMeshEntity : public Entity
	{
	 public:
		StaticMeshEntity(Window* window, const Reference<API::IVAO>& mesh, const Array<Reference<Material>>& mat = DEFAULT) :
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