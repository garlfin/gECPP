//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Entity.h"
#include "GL/Buffer/VAO.h"

namespace gE
{
	class MeshRenderer : public Component
	{
	 public:
		MeshRenderer(gE::Entity*, const GL::VAO* mesh);

		void OnUpdate(float delta) override;
		void OnRender(float delta) override;

		NODISCARD ALWAYS_INLINE const GL::Mesh& GetMeshSettings() { return _mesh->GetSettings(); }
	 private:
		const GL::VAO* _mesh;
	};
}