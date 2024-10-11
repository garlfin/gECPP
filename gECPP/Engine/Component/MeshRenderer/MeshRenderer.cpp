//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window.h>

#define DRAWCALL_TREE_INSERT(LIST, ITER_T, NEXT_ITER_T, COMP_FUNC) \
	if(similar && (similar = LIST.FindSimilar<T, COMP_FUNC>(t, searchFrom, SearchDirection::Right, searchTo))) \
	{ \
		searchFrom = &(**similar)->NEXT_ITER_T; \
		searchTo = similar->GetNext() ? &(**similar->GetNext())->NEXT_ITER_T : nullptr; \
	} \
	else \
	{ \
		LIST.Add((*t)->ITER_T); \
		searchFrom = searchTo = nullptr; \
	}


namespace gE
{
	inline GPU::IndirectDraw IndirectDrawArray[API_MAX_MULTI_DRAW];

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

	void DrawCallManager::OnRegister(Managed<DrawCall>& t)
	{
		using T = Managed<DrawCall>;

		ITER_T* searchFrom = nullptr, *searchTo = nullptr;
		ITER_T* similar;

		DRAWCALL_TREE_INSERT(_vaoList, _vaoIterator, _materialIterator, CompareVAO);
		DRAWCALL_TREE_INSERT(_materialList, _materialIterator, _subMeshIterator, CompareMaterial);
		DRAWCALL_TREE_INSERT(_submeshList, _subMeshIterator, _lodIterator, CompareMaterialIndex);
		DRAWCALL_TREE_INSERT(_lodList, _lodIterator, GetIterator(), CompareLOD);

		List.Insert(t.GetIterator(), searchFrom);
	}

	void DrawCallManager::OnRender(float d, Camera* camera)
	{
		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		u32 instanceCount = 0;
		u32 batchCount = 0;

		for(ITER_T* m = List.GetFirst(); m; m = m->GetNext())
		{
			DrawCall& call = ***m; // this is so stupid

			IndirectDrawArray[instanceCount] = GPU::IndirectDraw(1, call.GetMaterialIndex(), 0);

			GPU::ObjectInfo& object = buffers.Scene.Objects[0];

			buffers.Scene.InstanceCount = 1;
			buffers.Scene.State = window.State;
			object.Model = call.GetTransform().Model();
			object.PreviousModel = call.GetTransform().PreviousModel();
			object.Normal = inverse(call.GetTransform().Model());

			buffers.UpdateScene(offsetof(GPU::Scene, Objects[2]));

			call.GetMaterial()->Bind();
			call.GetVAO().Draw(1, IndirectDrawArray);
		}
	}

	DrawCall::DrawCall(DrawCallManager& manager, const MeshRenderer& r, Reference<Material>&& mat, u8 mesh) :
		Managed(&manager, *this, true),
		_vaoIterator(*this), _materialIterator(*this), _subMeshIterator(*this), _lodIterator(*this),
		_vao(&r.GetMesh()), _transform(&r.GetOwner().GetTransform()), _material(move(mat)), _subMesh(mesh), _lod(0)
	{
		Register();
	}
}
