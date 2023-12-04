//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

gE::MeshRenderer::MeshRenderer(gE::Entity* o, const gETF::MeshReference& mesh, const gE::MaterialHolder* mat)
	: Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh), _materialHolder(mat)
{
	if(_mesh->VAO) return;

	_mesh->CreateVAO(&GetWindow());
	_mesh->Free();
}

void gE::MeshRenderer::OnRender(float delta)
{
	// TODO: Switch to instanced rendering.
	DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

	buffers.Scene.InstanceCount = 1;
	buffers.Scene.Stage = (u32) GetWindow().Stage;
	buffers.Scene.Model[0] = GetOwner()->GetTransform().Model();
	buffers.Scene.PreviousModel[0] = GetOwner()->GetTransform().PreviousModel();
	buffers.Scene.Normal[0] = glm::mat3(1);

	buffers.UpdateScene();

	uint8_t meshCount = _mesh->MaterialCount;
	for(uint8_t i = 0; i < meshCount; i++)
	{
		_materialHolder->GetMaterial(i).Bind();
		_mesh->VAO->Draw(i);
	}
}

gE::Material& gE::MaterialHolder::GetMaterial(u8 i) const
{
	GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
	return _materials[i] || GetWindow().GetDefaultMaterial();
}