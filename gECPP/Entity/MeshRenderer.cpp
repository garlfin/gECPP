//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"

gE::MeshRenderer::MeshRenderer(gE::Window* window, const GL::VAO* mesh, gE::Entity* parent, const char* name) :
	Entity(window, parent, name), _mesh(mesh)
{

}

void gE::MeshRenderer::OnUpdate(float delta)
{
	Entity::OnUpdate(delta);
}

void gE::MeshRenderer::OnRender(float delta)
{
	uint8_t meshCount = _mesh->GetSettings()->MeshCount;
	for(uint8_t i = 0; i < meshCount; i++) _mesh->Draw(i);
}
