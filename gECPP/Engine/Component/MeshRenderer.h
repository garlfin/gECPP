//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "GL/Buffer/VAO.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(gE::Entity*, const gETF::MeshHandle& mesh);

		void OnUpdate(float delta) override;
		void OnRender(float delta) override;

		GET_CONST_VALUE(gETF::Mesh*, Mesh, _mesh.Get());

	 private:
		const gETF::MeshHandle _mesh;
	};
}