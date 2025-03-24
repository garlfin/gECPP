//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"

#include <Window.h>

#include "Core/Converter/MeshUtility.h"

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

	void MeshRenderer::SetMesh(const Reference<Mesh>& mesh)
	{
		if(_mesh == mesh) return;

		_mesh = mesh;
		UpdateDrawCalls();
	}

	void MeshRenderer::SetMaterial(u8 i, const Reference<Material>& mat)
	{
		GE_ASSERTM(i < _drawCalls.Size(), "MATERIAL OUT OF RANGE");
		if(mat.GetPointer() == _materials[i].GetPointer()) return;

		_materials[i] = mat;
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::SetMaterial(u8 i, Reference<Material>&& mat)
	{
		GE_ASSERTM(i < _drawCalls.Size(), "MATERIAL OUT OF RANGE");
		if(mat.GetPointer() == _materials[i].GetPointer()) return;
		_materials[i] = move(mat);
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::SetNullMaterial(u8 i)
	{
		GE_ASSERTM(i < _drawCalls.Size(), "MATERIAL OUT OF RANGE");
		if(!_materials[i]) return;

		_materials[i] = DEFAULT;
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::UpdateDrawCall(size_t i)
	{
		PlacementNew(_drawCalls[i], *this, i);
	}

	void MeshRenderer::UpdateDrawCalls()
	{
		for(u8 i = 0; i < GE_MAX_VAO_MATERIAL; i++)
			PlacementNew(_drawCalls[i], *this, i);
	}

	DragDropCompareFunc<Asset> MeshRenderer::GetDragDropAcceptor()
	{
		return AssetDragDropAcceptor<Mesh>;
	}

	REFLECTABLE_ONGUI_IMPL(MeshRenderer,
	   const size_t materialCount = _mesh->VAO->GetSettings().Counts.MaterialCount;

	   DrawField(AssetDragDropField<Mesh>{ "Mesh", "", GE_EDITOR_ASSET_PAYLOAD, GetDragDropAcceptor() }, *this, depth, GetMesh, SetMesh);
	   const size_t changed = DrawField(ArrayField<AssetDragDropField<Material>>{ "Materials" },
	       _materials.Data(), materialCount, depth);

	   if(changed != materialCount)
	   UpdateDrawCall(changed);
	)
	REFLECTABLE_FACTORY_NO_IMPL(MeshRenderer);

	Animator::Animator(const Reference<Skeleton>& skeleton) :
		_skeleton(skeleton)
	{

	}

	void Animator::Get(const Array<glm::mat4>& matrices) const
	{
		GE_ASSERT(matrices.Size() >= _transforms.Size());

		if(!_skeleton)
		{
			for(glm::mat4& matrix : matrices)
				matrix = glm::mat4(1.f);
			return;
		}

		for(size_t i = 0; i < _transforms.Size(); i++)
			_transforms[i] = _skeleton->Bones[i].Transform;

		if(_animation)
			_animation->Get(_time, _transforms);

		// underflows to be greater than _transforms.size()
		// backwards because it appears blender's gltfs store the hierarchy backwards
		for(size_t i = _transforms.Size() - 1; i < _transforms.Size(); i--)
			matrices[i] = _skeleton->Bones[i].InverseBindMatrix * _transforms[i].ToMat4();
	}

	void Animator::SetSkeleton(const Reference<Skeleton>& skeleton)
	{
		_skeleton = skeleton;
		if(_transforms.Size() != _skeleton->Bones.Size())
			_transforms = Array<TransformData>(_skeleton->Bones.Size());
	};

	bool Animator::DragDropAcceptor(const Reference<Asset>& asset, const Animator* animator)
	{
		return AssetDragDropAcceptor<Animation>(asset, nullptr) && ((Animation*) asset.GetPointer())->GetSkeleton() == animator->_skeleton;
	}

	REFLECTABLE_ONGUI_IMPL(Animator,
		DrawField(AssetDragDropField<Animation, Animator>{ "Animation", "", GE_EDITOR_ASSET_PAYLOAD, DragDropAcceptor, this }, _animation, depth);
		if(DrawField(AssetDragDropField<Skeleton>{ "Skeleton", "", GE_EDITOR_ASSET_PAYLOAD }, *this, depth, &Animator::GetSkeleton, &Animator::SetSkeleton))
			_animation = DEFAULT;
		DrawField(ScalarField{ "Time", "", 0.f }, _time, depth);
	);
	REFLECTABLE_FACTORY_NO_IMPL(Animator);

	AnimatedMeshRenderer::AnimatedMeshRenderer(Entity* owner, Animator* animator, const Reference<Mesh>& mesh) : MeshRenderer(owner, mesh),
		_animator(animator)
	{
		AnimatedMeshRenderer::SetMesh(mesh);
		UpdateDrawCalls();
	}

	void AnimatedMeshRenderer::OnUpdate(float delta)
	{
		if(!GetMesh()->Skeleton) return;

		const RendererManager& manager = GetWindow().GetRenderers();
		const API::ComputeShader& skinningShader = manager.GetSkinningShader();
		const API::IVAO& vao = *GetMesh()->VAO;
		const Array<glm::mat4>& jointBuf = manager.GetJoints().GetData();

		const API::Buffer<std::byte>& vertices = vao.GetBuffer(0);
		const API::Buffer<std::byte>& weights = vao.GetBuffer(1);
		const API::Buffer<std::byte>& verticesOut = _vao->GetBuffer(0);

		const size_t vertexCount = vertices->GetSize() / sizeof(Model::Vertex);

		vertices.Bind(GL::BufferBaseTarget::ShaderStorage, 9);
		weights.Bind(GL::BufferBaseTarget::ShaderStorage, 10);
		verticesOut.Bind(GL::BufferBaseTarget::ShaderStorage, 12);

		_animator->Get(jointBuf);
		manager.GetJoints().UpdateData();

		skinningShader.Dispatch(DIV_CEIL(vertexCount, 32));
	}

	GL::IVAO& AnimatedMeshRenderer::GetVAO() const
	{
		if(GetMesh()->Skeleton)
			return _vao;
		return MeshRenderer::GetVAO();
	}

	void AnimatedMeshRenderer::SetMesh(const Reference<Mesh>& mesh)
	{
		if(mesh == GetMesh()) return;

		GE_ASSERT((bool) mesh->VAO);
		const auto& meshVao = mesh->VAO;

		if(!mesh->VAO->GetSettings().IsFree())
			Log::Write("Warning: Mesh not freed before use in AnimatedMeshRenderer\n");

		if(dynamic_cast<API::VAO*>(mesh->VAO.GetPointer()))
			_vao = ptr_create<API::VAO>(&GetWindow(), meshVao->GetSettings());
		else
		{
			const API::IndexedVAO& meshIndexedVao = (API::IndexedVAO&) meshVao->GetSettings();
			const API::Buffer<std::byte>& meshIndices = meshIndexedVao.GetIndices();

			Pointer<API::IndexedVAO> vao = ptr_create<API::IndexedVAO>(&GetWindow(), meshIndexedVao.GetSettings());

			if(meshIndices.IsFree())
				glCopyNamedBufferSubData(meshIndices.Get(), vao->GetIndices().Get(), 0, 0, meshIndices->GetByteSize());

			_vao = move(vao);
		}

		MeshRenderer::SetMesh(mesh);
	}

	bool AnimatedMeshRenderer::DragDropAcceptor(const Reference<Asset>& asset, NoUserData userData)
	{
		return AssetDragDropAcceptor<Mesh>(asset, userData) && ((Mesh*) asset.GetPointer())->Skeleton;
	}

	REFLECTABLE_ONGUI_IMPL(AnimatedMeshRenderer,
	);
	REFLECTABLE_FACTORY_NO_IMPL(AnimatedMeshRenderer);

	RendererManager::RendererManager(Window* window): ComponentManager(window),
		_drawCallManager(window),
		_skinningShader(window, GPU::ComputeShader("Resource/Shader/Compute/skinning.comp")),
		_bonesBuffer(window, GE_MAX_BONES, nullptr, GPU::BufferUsageHint::Dynamic | GPU::BufferUsageHint::Write, true)
	{
		_bonesBuffer.Bind(API::BufferBaseTarget::ShaderStorage, 11);
	}

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

		_vao = &renderer.GetVAO();
		if(_vao)
			renderer.GetWindow().GetRenderers().GetDrawCallManager().Register(this);
	}

	DrawCall::~DrawCall()
	{
		if(_vao)
			_transform->GetWindow().GetRenderers().GetDrawCallManager().Remove(this);
	}

	void DrawCallManager::OnRender(float delta, const Camera* camera)
	{
		using ITER_T = SET_T::iterator;

		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();
		GPU::Scene& scene = **buffers.GetScene().GetData();

		scene.State = window.RenderState;

		u32 instanceCount = 0, batchInstanceCount = 0, batchCount = 0;
		for(ITER_T iter = _draws.begin(), nextIter; iter != _draws.end(); iter = nextIter)
		{
			GPU::ObjectInfo& object = scene.Objects[instanceCount];
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
				scene.InstanceCount[batchCount] = batchInstanceCount;
				_batches[batchCount] = GPU::IndirectDraw{ batchInstanceCount, draw->GetMaterialIndex(), draw->GetLOD() };

				batchCount++;
				batchInstanceCount = 0;
			}

			flush |= instanceCount == API_MAX_INSTANCE || batchCount == API_MAX_MULTI_DRAW;

			if(!flush) continue;

			const size_t updateTo = offsetof(GPU::Scene, Objects) + sizeof(GPU::ObjectInfo) * instanceCount;
			const Material* material = draw->GetMaterial() ? draw->GetMaterial() : &window.GetDefaultMaterial();

			buffers.GetScene().UpdateData<std::byte>(updateTo);
			material->Bind();
			draw->GetVAO().Draw(batchCount, _batches);

			batchCount = instanceCount = 0;
		}
	}
}
