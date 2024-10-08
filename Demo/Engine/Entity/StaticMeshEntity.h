//
// Created by scion on 10/6/2023.
//

#pragma once

#include <Engine/Component/MeshRenderer/MeshRenderer.h>
#include <Engine/Entity/Entity.h>

namespace gE::VoxelDemo
{
 	class StaticMeshEntity : public Entity
	{
	 public:
		StaticMeshEntity(Window* window, const Reference<API::IVAO>& mesh) :
			Entity(window),
			_renderer(this, mesh)
		{ };

		GET(gE::MeshRenderer&, Renderer, _renderer);

	 private:
		MeshRenderer _renderer;
	};
}