//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Entity.h"
#include <GL/Buffer/VAO.h>

namespace gE
{
	class MeshRenderer : public Entity
	{
	 public:
		MeshRenderer(gE::Window*, const GL::VAO* mesh, Entity* parent = nullptr, const char* = nullptr);

		void OnUpdate(float delta) override;
		void OnRender(float delta) override;

		NODISCARD ALWAYS_INLINE const GL::VAOSettings* GetMeshSettings() { return _mesh->GetSettings(); }
	 private:
		const GL::VAO* _mesh;
	};
}