//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <GL/Buffer/VAO.h>
#include <Engine/Renderer/Material.h>
#include <gETF/Mesh/Mesh.h>
#include "MaterialHolder.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* o, const Reference<gETF::Mesh>& mesh, const MaterialHolder& mat);

		void OnUpdate(float delta) override {};
		void OnRender(float delta, Camera*) override;

		GET_CONST(gETF::Mesh*, Mesh, _mesh.Get());
		GET_CONST(const MaterialHolder&, Materials, _materialHolder);

	 private:
		const Reference<gETF::Mesh> _mesh;
		const MaterialHolder& _materialHolder;
	};

	class RendererManager : public ComponentManager<MeshRenderer>
	{
	 public:
		using ComponentManager<MeshRenderer>::ComponentManager;

		void OnRender(float d, Camera* camera) override;

	 private:

	};
}