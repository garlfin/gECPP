//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

gE::MeshRenderer::MeshRenderer(gE::Entity* o, const Reference<gETF::Mesh>& mesh, const gE::MaterialHolder& mat) :
	Component(o, &o->GetWindow().GetRenderers()),
	_mesh(mesh), _materialHolder(mat)
{
	if(_mesh->VAO) return;

	_mesh->CreateVAO(&GetWindow());
	_mesh->Free();
}

void gE::MeshRenderer::OnRender(float delta, Camera*)
{
	DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

}

gE::Material& gE::MaterialHolder::GetMaterial(u8 i) const
{
	GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
	return _materials[i] || GetWindow().GetDefaultMaterial();
}

void gE::RendererManager::OnRender(float d, gE::Camera* camera)
{
	ComponentManager::OnRender(d, camera);

	for(Component* c : *this)
	{
		gE::Window& window = c->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		const MeshRenderer& info = *(MeshRenderer*) c;

		buffers.Scene.InstanceCount = 1;
		buffers.Scene.State = window.State;
		buffers.Scene.Model[0] = info.GetOwner()->GetTransform().Model();
		buffers.Scene.PreviousModel[0] = info.GetOwner()->GetTransform().PreviousModel();
		buffers.Scene.Normal[0] = glm::mat3(1);

		buffers.UpdateScene();

		uint8_t meshCount = info.GetMesh()->Materials.Count();
		for(uint8_t i = 0; i < meshCount; i++)
		{
			info.GetMaterials().GetMaterial(i).Bind();
			info.GetMesh()->VAO->Draw(i, window.State.InstanceMultiplier);
		}
	}
}
