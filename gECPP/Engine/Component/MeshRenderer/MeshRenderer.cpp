//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

namespace gE
{
	MeshRenderer::MeshRenderer(Entity* o, const Reference<API::IVAO>& mesh, const MaterialHolder& mat) :
		Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh), _materialHolder(mat),
		_drawCalls(mesh->GetData().Counts.MaterialCount)
	{
	}

	void MeshRenderer::OnRender(float delta, Camera*)
	{
		// DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();
	}

	Material& MaterialHolder::GetMaterial(u8 i) const
	{
		GE_ASSERT(i < GE_MAX_MATERIAL, "MATERIAL OUT OF RANGE");
		return _materials[i] || GetWindow().GetDefaultMaterial();
	}

	void RendererManager::OnRender(float d, Camera* camera)
	{
		IComponentManager::OnRender(d, camera);
		_drawCallManager.OnRender(d, camera);
	}
}
