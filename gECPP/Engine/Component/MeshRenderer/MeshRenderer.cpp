//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"
#include <Engine/Window/Window.h>

#define DRAWCALL_SUBITER_SAFE(VAR, ITER, FUNC) (VAR->ITER ? FUNC(VAR->ITER) : nullptr)
#define DRAWCALL_SIMILAR_SAFE(VAR, ITER) (VAR ? &VAR->ITER : nullptr)
#define DRAWCALL_DIRECTION (insertLocation ? Direction::Left : Direction::Right)

#define GE_ENABLE_BATCHING

namespace gE
{
	inline GPU::IndirectDraw IndirectDrawArray[API_MAX_MULTI_DRAW];

	typedef Managed<DrawCall> MAN_T;
	typedef LinkedIterator<MAN_T> ITER_T;
	typedef LinkedList<MAN_T> LIST_T;

	template<CompareFunc<MAN_T, MAN_T> FUNC, ITER_T DrawCall::* MEMBER>
	DrawCall* FindSimilarSafe(MAN_T& t, LIST_T& list, DrawCall* similar, DrawCall* next)
	{
		ITER_T* searchFrom = similar ? &(similar->*MEMBER) : nullptr;
		ITER_T* searchTo = next ? &(next->*MEMBER) : nullptr;
		ITER_T* found = list.FindSimilar<MAN_T, FUNC>(t, searchFrom, Direction::Right, searchTo);
		return found ? IPTR_TO_TPTR(found) : nullptr;
	}

	MeshRenderer::MeshRenderer(Entity* o, const Reference<Mesh>& mesh) :
		Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh),
		_drawCalls(mesh->VAO->GetData().Counts.MaterialCount)
	{
	#ifdef DEBUG
		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		for(u8 i = 0; i < _drawCalls.Count(); i++)
			SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, Reference<gE::Material>(), i);
	#endif
	}

	void MeshRenderer::OnRender(float delta, Camera*)
	{
		// DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();
	}

	void MeshRenderer::SetMaterial(u8 i, const Reference<Material>& mat)
	{
		GE_ASSERT(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
		if(mat.Get() == _drawCalls[i].GetMaterial()) return;

		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, COPY_MOVE(mat), i);
	}

	void MeshRenderer::SetMaterial(u8 i, Reference<Material>&& mat)
	{
		GE_ASSERT(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
		if(mat.Get() == _drawCalls[i].GetMaterial()) return;

		DrawCallManager& manager = GetWindow().GetRenderers().GetDrawCallManager();
		SAFE_CONSTRUCT(_drawCalls[i], DrawCall, manager, *this, move(mat), i);
	}

	void MeshRenderer::SetNullMaterial(u8 i)
	{
		GE_ASSERT(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
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
		DrawCall* similar;
		DrawCall* next = nullptr;
		DrawCall* insertLocation = nullptr;

		if(similar = FindSimilarSafe<CompareVAO, &DrawCall::_vaoIterator>(t, _vaoList, nullptr, nullptr))
		{
			next = DRAWCALL_SUBITER_SAFE(similar, _vaoIterator.GetNext(), IPTR_TO_TPTR);
			insertLocation = next;
		}
		else
			_vaoList.Add(t->_vaoIterator);

		if(similar && (similar = FindSimilarSafe<CompareMaterial, &DrawCall::_materialIterator>(t, _materialList, similar, next)))
		{
			next = DRAWCALL_SUBITER_SAFE(similar, _materialIterator.GetNext(), IPTR_TO_TPTR);
			insertLocation = next;
		}
		else
			_materialList.Insert(t->_materialIterator, DRAWCALL_SIMILAR_SAFE(insertLocation, _materialIterator), DRAWCALL_DIRECTION);

		if(similar && (similar = FindSimilarSafe<CompareSubMesh, &DrawCall::_subMeshIterator>(t, _subMeshList, similar, next)))
		{
			next = DRAWCALL_SUBITER_SAFE(similar, _subMeshIterator.GetNext(), IPTR_TO_TPTR);
			insertLocation = next;
		}
		else
			_subMeshList.Insert(t->_subMeshIterator, DRAWCALL_SIMILAR_SAFE(insertLocation, _subMeshIterator), DRAWCALL_DIRECTION);

		if(similar && (similar = FindSimilarSafe<CompareLOD, &DrawCall::_lodIterator>(t, _lodList, similar, next)))
		{
			next = DRAWCALL_SUBITER_SAFE(similar, _lodIterator.GetNext(), IPTR_TO_TPTR);
			insertLocation = next;
		}
		else
			_lodList.Insert(t->_lodIterator, DRAWCALL_SIMILAR_SAFE(insertLocation, _lodIterator), DRAWCALL_DIRECTION);

		List.Insert(t.GetIterator(), DRAWCALL_SIMILAR_SAFE(insertLocation, GetIterator()), DRAWCALL_DIRECTION);
	}

	void DrawCallManager::OnRender(float d, Camera* camera)
	{
		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		u32 instanceCount = 0, totalInstanceCount = 0, batchCount = 0;

		buffers.Scene.State = window.RenderState;

		for(ITER_T* m = List.GetFirst(); m; m = m->GetNext())
		{
			DrawCall& call = ***m; // this is so stupid
			DrawCall* nextCall = m->GetNext() ? &***m->GetNext() : nullptr;
			GPU::ObjectInfo& object = buffers.Scene.Objects[totalInstanceCount];

			object.Model = call.GetTransform().Model();
			object.PreviousModel = call.GetTransform().PreviousRenderModel();
			object.Normal = transpose(inverse(call.GetTransform().Model()));

		#ifdef GE_ENABLE_BATCHING
			totalInstanceCount++;
			instanceCount++;

			bool flush, flushBatch;
			if(nextCall)
			{
				flush = nextCall->_materialIterator.IsValid() || nextCall->_vaoIterator.IsValid();
				flushBatch = nextCall->_subMeshIterator.IsValid() || nextCall->_lodIterator.IsValid();
				// flush = call._material != nextCall->_material || call._vao != nextCall->_vao;
				// flushBatch = call._subMesh != nextCall->_subMesh || call._lod != nextCall->_lod;
			}
			else flush = flushBatch = true;

			flush |= totalInstanceCount == API_MAX_INSTANCE || batchCount == API_MAX_MULTI_DRAW;

			if(flushBatch)
			{
				buffers.Scene.InstanceCount[batchCount] = instanceCount;
				IndirectDrawArray[batchCount] = GPU::IndirectDraw(instanceCount, call.GetMaterialIndex(), 0);

				instanceCount = 0;
				batchCount++;
			}

			if(!flush) continue;

			u32 updateTo = offsetof(GPU::Scene, Objects) + sizeof(GPU::ObjectInfo) * totalInstanceCount;
			buffers.UpdateScene(updateTo);

			const Material* material = call.GetMaterial() ? call.GetMaterial() : &window.GetDefaultMaterial();

			material->Bind();
			call.GetVAO().Draw(batchCount, IndirectDrawArray);

			batchCount = totalInstanceCount = instanceCount = 0;
		#else
			buffers.Scene.InstanceCount[0] = instanceCount;
			buffers.UpdateScene(offsetof(GPU::Scene, Objects[1]));

			(call.GetMaterial() ?: &window.GetDefaultMaterial())->Bind();
			call.GetVAO().Draw(call._subMesh, window.State.InstanceMultiplier);
		#endif
		}
	}

	DrawCall::DrawCall(DrawCallManager& manager, const MeshRenderer& r, Reference<Material>&& mat, u8 mesh) :
		Managed(&manager, *this, true),
		_vao(r.GetMesh().VAO), _transform(&r.GetOwner().GetTransform()), _material(move(mat)), _subMesh(mesh),
		_lod(0), _vaoIterator(*this), _materialIterator(*this), _subMeshIterator(*this), _lodIterator(*this)
	{
		Register();
	}
}
