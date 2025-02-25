//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"

#include <Window.h>

#define DRAWCALL_SUBITER_SAFE(VAR, ITER, FUNC) (VAR->ITER ? FUNC(VAR->ITER) : nullptr)
#define DRAWCALL_SIMILAR_SAFE(VAR, ITER) (VAR ? &VAR->ITER : nullptr)
#define DRAWCALL_DIRECTION (insertLocation ? Direction::Left : Direction::Right)

#define GE_ENABLE_BATCHING

namespace gE
{
	MeshRenderer::MeshRenderer(Entity* o, const Reference<Mesh>& mesh) :
		Component(o, &o->GetWindow().GetRenderers()), _mesh(mesh),
		_drawCalls(GE_MAX_VAO_MATERIAL),
		_materials(GE_MAX_VAO_MATERIAL)
	{
		for(u8 i = 0; i < GE_MAX_VAO_MATERIAL; i++)
		{
			_materials[i] = DEFAULT;
			PlacementNew(_drawCalls[i], *this, i);
		}
	}

	void MeshRenderer::OnRender(float delta, Camera*)
	{
		// DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();
	}

	void MeshRenderer::SetMesh(const Reference<Mesh>& mesh)
	{
		if(_mesh == mesh) return;

		_mesh = mesh;
		for(u8 i = 0; i < GE_MAX_VAO_MATERIAL; i++)
			PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::SetMaterial(u8 i, const Reference<Material>& mat)
	{
		GE_ASSERTM(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
		if(mat.GetPointer() == _materials[i].GetPointer()) return;

		_materials[i] = mat;
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::SetMaterial(u8 i, Reference<Material>&& mat)
	{
		GE_ASSERTM(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
		if(mat.GetPointer() == _materials[i].GetPointer()) return;
		_materials[i] = move(mat);
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::SetNullMaterial(u8 i)
	{
		GE_ASSERTM(i < _drawCalls.Count(), "MATERIAL OUT OF RANGE");
		if(!_materials[i]) return;

		_materials[i] = DEFAULT;
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::UpdateDrawCall(size_t i)
	{
		PlacementNew(_drawCalls[i], *this, i);
	}

	REFLECTABLE_FACTORY_NO_IMPL(MeshRenderer);

	void MeshRenderer::IOnEditorGUI(u8 depth)
	{
		const size_t materialCount = _mesh->VAO->GetSettings().Counts.MaterialCount;

		DrawField(Field{ "Mesh" }, *this, depth, GetMesh, SetMesh);
		const size_t changed = DrawField(ArrayField<Field>{ "Materials" }, _materials.Data(), materialCount, depth);

		if(changed != materialCount)
			UpdateDrawCall(changed);
	};

	void RendererManager::OnRender(float d, Camera* camera)
	{
		IComponentManager::OnRender(d, camera);
		_drawCallManager.OnRender(d, camera);
	}

	DrawCall::DrawCall(const MeshRenderer& renderer, u8 i) :
		_transform(&renderer.GetOwner().GetTransform()),
		_material(renderer.GetMaterial(i)),
		_materialIndex(i),
		_lod(0)
	{
		if(i >= renderer.GetMesh()->VAO->GetSettings().Counts.MaterialCount) return;

		_vao = renderer.GetMesh()->VAO.GetPointer();
		renderer.GetWindow().GetRenderers().GetDrawCallManager().Register(this);
	}

	DrawCall::~DrawCall()
	{
		if(!_vao) return;

		_transform->GetWindow().GetRenderers().GetDrawCallManager().Remove(this);
	}

	void DrawCallManager::OnRender(float delta, const Camera* camera)
	{
		using ITER_T = SET_T::iterator;

		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		buffers.Scene.State = window.RenderState;

		u32 instanceCount = 0, batchInstanceCount = 0, batchCount = 0;
		for(ITER_T iter = _draws.begin(), nextIter; iter != _draws.end(); iter = nextIter)
		{
			GPU::ObjectInfo& object = buffers.Scene.Objects[instanceCount];
			const DrawCall* draw = *iter;
			bool flush = true;
			bool flushBatch = true;

			nextIter = ++COPY(iter);
			if(nextIter != _draws.end())
			{
				const DrawCall* nextDraw = *nextIter;

				flush = &draw->GetVAO() != &nextDraw->GetVAO() || draw->GetMaterial() != nextDraw->GetMaterial();
				flushBatch = draw->GetMaterialIndex() != nextDraw->GetMaterialIndex() || draw->GetLOD() != nextDraw->GetMaterialIndex();
			}

			object.Model = draw->GetTransform().Model();
			object.PreviousModel = draw->GetTransform().PreviousRenderModel();
			object.Normal = glm::transpose(glm::inverse(draw->GetTransform().Model()));

			instanceCount++;
			batchInstanceCount++;

			if(flushBatch || flush)
			{
				buffers.Scene.InstanceCount[batchCount] = batchInstanceCount;
				batches[batchCount] = GPU::IndirectDraw{ batchInstanceCount, draw->GetMaterialIndex(), draw->GetLOD() };

				batchCount++;
				batchInstanceCount = 0;
			}

			flush |= instanceCount == API_MAX_INSTANCE || batchCount == API_MAX_MULTI_DRAW;

			if(!flush) continue;

			const u32 updateTo = offsetof(GPU::Scene, Objects) + sizeof(GPU::ObjectInfo) * instanceCount;
			const Material* material = draw->GetMaterial() ? draw->GetMaterial() : &window.GetDefaultMaterial();

			buffers.UpdateScene(updateTo);
			material->Bind();
			draw->GetVAO().Draw(batchCount, batches);

			batchCount = instanceCount = 0;
		}
	}
}
