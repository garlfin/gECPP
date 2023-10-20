//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

gE::MeshRenderer::MeshRenderer(gE::Entity* owner, const gETF::MeshHandle& mesh, const Handle<Material>& mat)
	: Component(owner), _mesh(mesh), _mat(mat)
{
	GET_WINDOW()->GetRenderers().Register(this);
	if(mesh->VAO) return;

	mesh->CreateVAO(GET_WINDOW());
	mesh->Free();
}

void gE::MeshRenderer::OnUpdate(float delta)
{

}

void gE::MeshRenderer::OnRender(float delta)
{
	Window* window = GET_WINDOW();
	DefaultPipeline::Buffers* buffers = window->GetPipelineBuffers();

	buffers->Scene.InstanceCount = 1;
	buffers->Scene.Model[0] = Owner()->GetTransform().Model();
	buffers->Scene.Normal[0] = glm::mat3(1);

	buffers->UpdateScene(offsetof(GL::Scene, Normal[1]));

	_mat->Bind();

	uint8_t meshCount = _mesh->MaterialCount;
	for (uint8_t i = 0; i < meshCount; i++) _mesh->VAO->Draw(i);
}