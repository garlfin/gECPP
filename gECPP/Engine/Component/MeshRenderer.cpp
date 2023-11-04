//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

gE::MeshRenderer::MeshRenderer(gE::Entity* owner, const gETF::MeshReference& mesh, const gE::MaterialHolder* mat)
	: Component(owner), _mesh(mesh), _materialHolder(mat)
{
	GET_WINDOW()->GetRenderers().Register(this);
	if(_mesh->VAO) return;

	_mesh->CreateVAO(GET_WINDOW());
	_mesh->Free();
}

void gE::MeshRenderer::OnUpdate(float delta)
{

}

void gE::MeshRenderer::OnRender(float delta)
{
	Window* window = GET_WINDOW();
	DefaultPipeline::Buffers& buffers = window->GetPipelineBuffers();

	buffers.Scene.InstanceCount = 1;
	buffers.Scene.Model[0] = Owner()->GetTransform().Model();
	buffers.Scene.Normal[0] = glm::mat3(1);
	buffers.UpdateScene(offsetof(GL::Scene, Normal[1]), offsetof(GL::Scene, InstanceCount));

	uint8_t meshCount = _mesh->MaterialCount;
	for (uint8_t i = 0; i < meshCount; i++)
	{
		_materialHolder->GetMaterial(i).Bind();
		_mesh->VAO->Draw(i);
	}
}