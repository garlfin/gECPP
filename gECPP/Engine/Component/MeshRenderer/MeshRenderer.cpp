//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

namespace gE
{
	MeshRenderer::MeshRenderer(Entity* o, const Reference<API::IVAO>& mesh) :
		Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh),
		_drawCalls(mesh->GetData().Counts.MaterialCount)
	{
	}

	void MeshRenderer::OnRender(float delta, Camera*)
	{
		// DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();
	}

	void MeshRenderer::SetMaterial(u8 i, const Reference<Material>& mat)
	{
		GE_ASSERT(i < GE_MAX_VAO_MATERIAL, "MATERIAL OUT OF RANGE");
		if(mat.Get() == _drawCalls[i].GetMaterial()) return;

		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, COPY_MOVE(mat), i);
	}

	void MeshRenderer::SetMaterial(u8 i, Reference<Material>&& mat)
	{
		GE_ASSERT(i < GE_MAX_VAO_MATERIAL, "MATERIAL OUT OF RANGE");
		if(mat.Get() == _drawCalls[i].GetMaterial()) return;

		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, move(mat), i);
	}

	void MeshRenderer::SetNullMaterial(u8 i)
	{
		GE_ASSERT(i < GE_MAX_VAO_MATERIAL, "MATERIAL OUT OF RANGE");
		if(!_drawCalls[i].GetMaterial()) return;

		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, Reference<Material>(nullptr), i);
	}

	void RendererManager::OnRender(float d, Camera* camera)
	{
		IComponentManager::OnRender(d, camera);
		_drawCallManager.OnRender(d, camera);
	}

	void DrawCallManager::OnRender(float d, Camera* camera)
	{
		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		u32 batchCount = 0;
		for(ITER_T* m = List.GetFirst(); m; m = m->GetNext())
		{
			DrawCall& call = ***m; // this is so stupid

			const MeshRenderer& renderer = call.GetRenderer();
			const API::IVAO& vao = renderer.GetMesh();
			const Transform& transform = renderer.GetOwner().GetTransform();

			buffers.Scene.InstanceCount = 1;
			buffers.Scene.State = window.State;
			buffers.Scene.Model[0] = transform.Model();
			buffers.Scene.PreviousModel[0] = transform.PreviousModel();
			buffers.Scene.Normal[0] = inverse(transform.Model());

			buffers.UpdateScene();

			call.GetMaterial()->Bind();
			vao.Draw(1, window.State.InstanceMultiplier);
		}
	}
}
