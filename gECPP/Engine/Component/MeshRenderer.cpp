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

	_instance.Mesh = GetMesh();
	_instance.Flags = InstanceFlags{ 0, false, true };
	_instance.Model = &GetOwner()->GetTransform().Model();
	_instance.PreviousModel = &GetOwner()->GetTransform().PreviousModel();
	_instance.Layer = UINT8_MAX;

	for(u8 i = 0; i < GE_MAX_MATERIAL; i++)
		_instance.Materials[i] = &_materialHolder.GetMaterial(i);
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

		const InstanceInfo& info = ((MeshRenderer*) c)->GetInstanceInfo();

		buffers.Scene.InstanceCount = 1;
		buffers.Scene.State = window.State;
		buffers.Scene.Model[0] = *info.Model;
		buffers.Scene.PreviousModel[0] = *info.PreviousModel;
		buffers.Scene.Normal[0] = glm::mat3(1);

		buffers.UpdateScene();

		uint8_t meshCount = info.Mesh->Materials.Count();
		for(uint8_t i = 0; i < meshCount; i++)
		{
			info.Materials[i]->Bind();
			info.Mesh->VAO->Draw(i, window.State.InstanceMultiplier);
		}
	}
}
