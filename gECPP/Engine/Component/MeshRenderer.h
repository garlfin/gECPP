//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <GL/Buffer/VAO.h>
#include <Engine/Renderer/Material.h>
#include <gEModel/gETF.h>

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(Entity* owner, const gETF::MeshHandle& mesh, const Handle<Material>& mat);

		void OnUpdate(float delta) override;
		void OnRender(float delta) override;

		GET_CONST_VALUE(gETF::Mesh*, Mesh, _mesh.Get());

	 private:
		const gETF::MeshHandle _mesh;
		const Handle<Material> _mat;
	};
}