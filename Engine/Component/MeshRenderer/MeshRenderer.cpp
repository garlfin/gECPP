//
// Created by scion on 9/5/2023.
//

#include "MeshRenderer.h"

#include <Window.h>

#include <Core/Converter/MeshUtility.h>
#include <Entity/Light/DirectionalLight.h>

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

	template<class T, u64 SORT_COUNT> requires std::is_base_of_v<Entity, T>
	SmallVector<const T*, SORT_COUNT> SortClosest(const LinkedList<Managed<T>>& list,  const vec3& position, bool(*predicate)(const T& t) = nullptr)
	{
		using pair = std::pair<const T*, float>;
		using vec = SmallVector<pair, SORT_COUNT>;

		vec sorted {};
		for(const auto& managed : list)
		{
			const T& component = **managed;
			const TransformData& globalTransform = component.GetTransform().GetGlobalTransform();

			if(predicate && !predicate(component))
				continue;

			float distance = distance2(globalTransform.Position, position);

			typename vec::iterator foundIt = std::find_if(sorted.begin(), sorted.end(), [&](const pair& a){ return a.second > distance; });
			sorted.Insert(foundIt, pair(&component, distance));
		}

		SmallVector<const T*, SORT_COUNT> result{};
		for(const pair& p : sorted)
			result.PushBack(p.first);
		return result;
	}

	void MeshRenderer::OnUpdate(float delta)
	{
		const Transform& transform = GetOwner().GetTransform();
		if(!(bool)(transform.GetFlags() & TransformFlags::RenderInvalidated))
			return;

		const TransformData& globalTransform = transform.GetGlobalTransform();

		_nearestCubemaps = SortClosest<CubemapCapture, 4>(GetWindow().GetCubemaps().GetList(), globalTransform.Position);
		_nearestLights = SortClosest<Light, 4>(GetWindow().GetLights().GetList(), globalTransform.Position, [](const Light& light){ return light.GetType() != &DirectionalLight::SType; });
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

	void MeshRenderer::GetGPUObject(GPU::ObjectInfo& object) const
	{
		Transform& transform = GetOwner().GetTransform();

		object.Model = transform.Model();
		object.PreviousModel = transform.PreviousRenderModel();
		object.Normal = transpose(inverse(transform.Model()));
		object.Flags = GetFlags();
	}

	void MeshRenderer::GetGPULighting(GPU::ObjectLighting& lighting) const
	{
		lighting.LightCount = _nearestLights.Size();
		for(u64 i = 0; i < _nearestLights.Size(); i++)
			_nearestLights[i]->GetGPULight(lighting.Lights[i]);

		lighting.CubemapCount = _nearestCubemaps.Size();
		for(u64 i = 0; i < _nearestCubemaps.Size(); i++)
			_nearestCubemaps[i]->GetGPUCubemap(lighting.Cubemaps[i]);
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

		DrawField(AssetDragDropField<Mesh>{ "Mesh", "", GE_EDITOR_ASSET_PAYLOAD, GetDragDropAcceptor() }, *this, depth, &MeshRenderer::GetMesh, &MeshRenderer::SetMesh);
		const size_t changed = DrawField(ArrayField<AssetDragDropField<Material>>{ "Materials" }, _materials.Data(), materialCount, depth);

		if(changed != materialCount)
			UpdateDrawCall(changed);
	)
	REFLECTABLE_FACTORY_NO_IMPL(MeshRenderer);

	IAnimator::IAnimator(const Reference<Skeleton>& skeleton) :
		_skeleton(skeleton)
	{
	}

	void IAnimator::Get(float delta, const Array<mat4>& matrices)
	{
		GE_ASSERT(matrices.Size() >= _transforms.Size());

		if(!_skeleton)
		{
			for(mat4& matrix : matrices)
				matrix = mat4(1.f);
			return;
		}

		for(size_t i = 0; i < _skeleton->Bones.Size(); i++)
			_transforms[i] = _skeleton->Bones[i].Transform;

		OnUpdate(delta);

		for(size_t i = 0; i < _skeleton->Bones.Size(); i++)
			if(const Bone* const parent = _skeleton->Bones[i].Parent.Pointer)
				matrices[i] = matrices[_skeleton->GetIndex(*parent)] * _transforms[i].ToMat4();
			else
				matrices[i] = _transforms[i].ToMat4();

		for(size_t i = 0; i < _skeleton->Bones.Size(); i++)
			matrices[i] = matrices[i] * _skeleton->Bones[i].InverseBindMatrix;
	}

	void IAnimator::SetSkeleton(const Reference<Skeleton>& skeleton)
	{
		_skeleton = skeleton;
		if(_transforms.Size() != _skeleton->Bones.Size())
			_transforms = Array<TransformData>(_skeleton->Bones.Size());
	}

	bool IAnimator::DragDropAcceptor(const Reference<Asset>& asset, const IAnimator* animator)
	{
		return AssetDragDropAcceptor<Animation>(asset, nullptr) && ((Animation*) asset.GetPointer())->GetSkeleton();
	}

	REFLECTABLE_ONGUI_IMPL(IAnimator,
		DrawField(AssetDragDropField<Skeleton>{ "Skeleton", "", GE_EDITOR_ASSET_PAYLOAD }, *this, depth, &IAnimator::GetSkeleton, &IAnimator::SetSkeleton);
	);
	REFLECTABLE_FACTORY_NO_IMPL(IAnimator);

	void SimpleAnimator::OnUpdate(float delta)
	{
		_time += delta;

		if(_animation)
			_animation->Get(_time, TransformMixMode::Override, 1.f, GetTransforms());
	}

	REFLECTABLE_ONGUI_IMPL(SimpleAnimator,
		if(DrawField(AssetDragDropField<Animation, IAnimator>{ "Animation", "", GE_EDITOR_ASSET_PAYLOAD, DragDropAcceptor, this }, _animation, depth))
			SetSkeleton(_animation->GetSkeleton());
		DrawField(ScalarField{ "Time", "", 0.f }, _time, depth);
	);
	REFLECTABLE_FACTORY_NO_IMPL(SimpleAnimator);

	AnimatedMeshRenderer::AnimatedMeshRenderer(Entity* owner, IAnimator* animator, const Reference<Mesh>& mesh) : MeshRenderer(owner, mesh),
		_animator(animator)
	{
		AnimatedMeshRenderer::SetMesh(mesh);
		UpdateDrawCalls();
	}

	void AnimatedMeshRenderer::OnRender(float delta, Camera* camera)
	{
#ifdef GE_ENABLE_EDITOR
		if(GetWindow().RenderState.RenderMode != RenderMode::Fragment) return;
		if(!GetMesh()->Skeleton || !_enableDebugView) return;

		const RendererManager& manager = GetWindow().GetRenderers();
		const DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

		_animator->Get(0.f, manager.GetJoints().GetData());
		for(mat4& matrix : manager.GetJoints())
			matrix = inverse(matrix);

		manager.GetJoints().UpdateData(GetMesh()->Skeleton->Bones.Size());

		buffers.GetScene().GetData()[0].Objects[0] = GPU::ObjectInfo{ GetOwner().GetTransform().Model() };
		buffers.GetScene().UpdateData<GPU::ObjectInfo>(1, offsetof(GPU::Scene, Objects));

		glDepthFunc(GL_ALWAYS);
		manager.GetBoneDebugShader().Bind();
		manager.GetBoneDebugVAO().Draw(0, (u16) GetMesh()->Skeleton->Bones.Size());
#endif
	}

	void AnimatedMeshRenderer::OnUpdate(float delta)
	{
		MeshRenderer::OnUpdate(delta);

		if(!GetMesh()->Skeleton) return;

		const RendererManager& manager = GetWindow().GetRenderers();
		const API::ComputeShader& skinningShader = manager.GetSkinningShader();
		const API::IVAO& vao = *GetMesh()->VAO;
		const Array<mat4>& jointBuf = manager.GetJoints().GetData();

		const API::Buffer<std::byte>& vertices = vao.GetBuffer(0);
		const API::Buffer<std::byte>& weights = vao.GetBuffer(1);
		const API::Buffer<std::byte>& verticesOut = _vao->GetBuffer(0);
		const API::Buffer<std::byte>& previousPositionOut = _vao->GetBuffer(1);

		const size_t vertexCount = vertices->GetSize() / sizeof(Model::Vertex);

		vertices.Bind(GL::BufferBaseTarget::ShaderStorage, 9);
		weights.Bind(GL::BufferBaseTarget::ShaderStorage, 10);
		verticesOut.Bind(GL::BufferBaseTarget::ShaderStorage, 12);
		previousPositionOut.Bind(GL::BufferBaseTarget::ShaderStorage, 13);

		_animator->Get(delta, jointBuf);
		manager.GetJoints().UpdateData(GetMesh()->Skeleton->Bones.Size());

		skinningShader.Dispatch(DIV_CEIL(vertexCount, 32));
	}

	GL::IVAO* AnimatedMeshRenderer::GetVAO() const
	{
		if(GetMesh()->Skeleton)
			return _vao.GetPointer();
		return MeshRenderer::GetVAO();
	}

	GPU::ObjectFlags AnimatedMeshRenderer::GetFlags() const
	{
		return
		{
			GetMesh()->Skeleton && _animator->GetSkeleton()
		};
	}

	void AnimatedMeshRenderer::SetMesh(const Reference<Mesh>& mesh)
	{
		if(mesh == GetMesh()) return;

		GE_ASSERT((bool) mesh->VAO);
		const auto& meshVao = mesh->VAO;

		if(!mesh->VAO->GetSettings().IsFree())
			Log::Write("Warning: Mesh not freed before use in AnimatedMeshRenderer\n");

		if(dynamic_cast<API::VAO*>(mesh->VAO.GetPointer()))
		{
			GPU::VAO settings = meshVao->GetSettings();
			AddPreviousPositionField(settings, settings.Buffers[0].GetSize() / sizeof(Model::Vertex));

			_vao = ptr_create<API::VAO>(&GetWindow(), move(settings));
		}
		else
		{
			const API::IndexedVAO& meshIndexedVao = (API::IndexedVAO&) meshVao->GetSettings();
			const API::Buffer<std::byte>& meshIndices = meshIndexedVao.GetIndices();

			GPU::IndexedVAO settings = meshIndexedVao.GetSettings();
			AddPreviousPositionField(settings, settings.Buffers[0].GetSize() / sizeof(Model::Vertex));

			Pointer<API::IndexedVAO> vao = ptr_create<API::IndexedVAO>(&GetWindow(), move(settings));

			if(meshIndices.IsFree())
				glCopyNamedBufferSubData(meshIndices.Get(), vao->GetIndices().Get(), 0, 0, meshIndices->GetByteSize());

			_vao = move(vao);
		}

		MeshRenderer::SetMesh(mesh);
	}

	bool AnimatedMeshRenderer::DragDropAcceptor(const Reference<Asset>& asset, NoUserData userData)
	{
		return AssetDragDropAcceptor<Mesh>(asset, userData);// && ((Mesh*) asset.GetPointer())->Skeleton;
	}

	void AnimatedMeshRenderer::AddPreviousPositionField(GPU::VAO& vao, size_t vertexCount)
	{
		static GPU::VertexField previousPositionField { "PVRP", GPU::ElementType::Float, false, 1, 4, 3, 0};

		vao.AddField(previousPositionField);
		vao.Buffers[1] = GPU::Buffer<std::byte>(sizeof(vec4) * vertexCount, nullptr, sizeof(vec4), GPU::BufferUsageHint::Dynamic, false);
	}

	REFLECTABLE_ONGUI_IMPL(AnimatedMeshRenderer,
	    DrawField(Field{ "Enable Debug Skeleton" }, _enableDebugView, depth);
	);
	REFLECTABLE_FACTORY_NO_IMPL(AnimatedMeshRenderer);

	RendererManager::RendererManager(Window* window): ComponentManager(window),
		_drawCallManager(window),
		_skinningShader(window, GPU::ComputeShader("Resource/Shader/Compute/skinning.comp")),
		_bonesBuffer(window, GE_MAX_BONES, nullptr, GPU::BufferUsageHint::Dynamic | GPU::BufferUsageHint::Write, true),
		_boneDebugVAO(window, BoneDebugVAOFormat),
		_boneDebugShader(window, GPU::Shader("Resource/Shader/bone.vert", "Resource/Shader/wireframe.frag")),
		_wireframeShader(window, GPU::Shader("Resource/Shader/wireframe.vert", "Resource/Shader/wireframe.frag"))
	{

		_bonesBuffer.Bind(API::BufferBaseTarget::ShaderStorage, 11);
	}

	void RendererManager::OnRender(float d, Camera* camera)
	{
		_drawCallManager.OnRender(d, camera);
		IComponentManager::OnRender(d, camera);
	}

	DrawCall::DrawCall(const MeshRenderer& renderer, u8 i) :
		_renderer(&renderer),
		_material(renderer.GetMaterial(i)),
		_submeshIndex(i),
		_lod(0),
		_vao(renderer.GetVAO())
	{
		if(!_vao) return;
		if(i >= renderer.GetMesh()->VAO->GetSettings().Counts.MaterialCount) return;
		renderer.GetWindow().GetRenderers().GetDrawCallManager().Register(this);
	}

	DrawCall::~DrawCall()
	{
		if(_vao)
			_renderer->GetWindow().GetRenderers().GetDrawCallManager().Remove(this);
	}

	void DrawCallManager::OnRender(float delta, const Camera* camera)
	{
		using ITER_T = SET_T::iterator;

		Window& window = camera->GetWindow();
		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();
		GPU::Scene& scene = **buffers.GetScene().GetData();
		GPU::Lighting& lighting = **buffers.GetLights().GetData();

		scene.State = window.RenderState;

		size_t instanceCount = 0, batchInstanceCount = 0, batchCount = 0;
		for(ITER_T iter = _draws.begin(), nextIter; iter != _draws.end(); iter = nextIter)
		{
			const DrawCall* draw = *iter;
			bool flush = true;
			bool flushBatch = true;

			const MeshRenderer& renderer = draw->GetRenderer();
			const Transform& transform = renderer.GetOwner().GetTransform();
			const Material* material = draw->GetMaterial() ? draw->GetMaterial() : &window.GetDefaultMaterial();
			const Shader* shader = draw->GetShader();

			nextIter = ++COPY(iter);
			if(nextIter != _draws.end())
			{
				const DrawCall* nextDraw = *nextIter;

				flush = &draw->GetVAO() != &nextDraw->GetVAO() || draw->GetShader() != nextDraw->GetShader();
				flushBatch = draw->GetSubmeshIndex() != nextDraw->GetSubmeshIndex() || draw->GetLOD() != nextDraw->GetSubmeshIndex();
			}

			renderer.GetGPUObject(scene.Objects[instanceCount]);
			renderer.GetGPULighting(lighting.Objects[instanceCount]);

			if(material)
				material->GetGPUMaterialData(instanceCount);

			instanceCount++;
			batchInstanceCount++;

			if(flushBatch || flush)
			{
				scene.InstanceCount[batchCount] = batchInstanceCount;
				_batches[batchCount] = GPU::IndirectDraw{ (u32) batchInstanceCount, draw->GetSubmeshIndex(), draw->GetLOD() };

				batchCount++;
				batchInstanceCount = 0;
			}

			flush |= instanceCount == API_MAX_INSTANCE || batchCount == API_MAX_MULTI_DRAW;

			if(!flush) continue;

			const size_t sceneUpdateSize = offsetof(GPU::Scene, Objects) + sizeof(GPU::ObjectInfo) * instanceCount;

			buffers.GetScene().UpdateData<std::byte>(sceneUpdateSize);
			buffers.GetLights().UpdateData<GPU::ObjectLighting>(instanceCount, offsetof(GPU::Lighting, Objects));

			if(material)
			{
				material->FlushMaterialData(instanceCount);
				material->Bind();
			}
			else
				window.GetDefaultMaterial().Bind();

			draw->GetVAO().Draw(batchCount, _batches);

			batchCount = instanceCount = 0;
		}
	}
}
