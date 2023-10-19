//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>
#include <utility>

gE::MeshRenderer::MeshRenderer(gE::Entity* owner, const gETF::MeshHandle& mesh)
	: Component(owner), _mesh(mesh)
{
	GET_WINDOW()->GetRenderers().Register(this);
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

	uint8_t meshCount = _mesh->MaterialCount;
	for (uint8_t i = 0; i < meshCount; i++) _mesh->Draw(i);
}
