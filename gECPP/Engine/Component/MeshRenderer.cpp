//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"

gE::MeshRenderer::MeshRenderer(gE::Entity* owner, const GL::VAO* mesh) :
	Component(owner), _mesh(mesh)
{

}

void gE::MeshRenderer::OnUpdate(float delta)
{

}

void gE::MeshRenderer::OnRender(float delta)
{
	uint8_t meshCount = _mesh->GetSettings().MeshCount;
	for (uint8_t i = 0; i < meshCount; i++) _mesh->Draw(i);
}