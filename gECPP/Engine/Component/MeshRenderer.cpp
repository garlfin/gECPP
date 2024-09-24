//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

namespace gE
{
	MeshRenderer::MeshRenderer(Entity* o, const Reference<API::IVAO>& mesh, const MaterialHolder& mat) :
		Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh), _materialHolder(mat)
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
		ComponentManager::OnRender(d, camera);

		for(Managed<Component>* m = List.GetFirst(); m; m = m->GetNext())
		{
			MeshRenderer& renderer = *(MeshRenderer*) &m->Get();
			Window& window = renderer.GetWindow();
			DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

			const API::IVAO& vao = renderer.GetMesh();
			const GPU::VAO& vaoData = vao.GetData();
			const Transform& transform = renderer.GetOwner().GetTransform();

			buffers.Scene.InstanceCount = 1;
			buffers.Scene.State = window.State;
			buffers.Scene.Model[0] = transform.Model();
			buffers.Scene.PreviousModel[0] = transform.PreviousModel();
			buffers.Scene.Normal[0] = glm::mat3(1);

			buffers.UpdateScene();

			uint8_t meshCount = vaoData.Counts.MaterialCount;
			for(uint8_t i = 0; i < meshCount; i++)
			{
				renderer.GetMaterials().GetMaterial(i).Bind();
				vao.Draw(i, window.State.InstanceMultiplier);
			}
		}
	}
}
