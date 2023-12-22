//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <GL/Buffer/VAO.h>
#include <Engine/Renderer/Material.h>
#include <gEModel/gETF.h>
#include "MaterialHolder.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* o, const gETF::MeshReference& mesh, const MaterialHolder* mat);

		void OnUpdate(float delta) override {};
		void OnRender(float delta) override;

		GET_CONST(gETF::Mesh*, Mesh, _mesh.Get());

	 private:
		const gETF::MeshReference _mesh;
		const MaterialHolder* _materialHolder;
	};
}