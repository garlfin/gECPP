//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Core/Material/Material.h>
#include <Core/Mesh/Mesh.h>
#include <Entity/Entity.h>
#include <Graphics/Buffer/VAO.h>

#include "DrawCall.h"
#include "Core/Pipeline/Buffers.h"

namespace gE
{
    class MeshRenderer : public Component
    {
        REFLECTABLE_PROTO("MREN", MeshRenderer, Component);

    public:
        MeshRenderer(Entity* o, const Reference<Mesh>& mesh);

        void OnInit() override {};
        void OnUpdate(float delta) override {};

        GET_CONST(size_t, MaterialCount, _materials.Size());
        GET_CONST(const Reference<Mesh>&, Mesh, _mesh);
        virtual void SetMesh(const Reference<Mesh>& mesh);

        NODISCARD ALWAYS_INLINE Material* GetMaterial(u8 i) const { return _materials[i].GetPointer(); }

        void SetMaterial(u8 i, const Reference<Material>& mat);
        void SetMaterial(u8 i, Reference<Material>&& mat);
        void SetNullMaterial(u8 i);

        virtual API::IVAO* GetVAO() const { return _mesh ? _mesh->VAO.GetPointer() : nullptr; }
        virtual GPU::ObjectFlags GetFlags() const { return DEFAULT; }

        friend class RendererManager;

    protected:
        void UpdateDrawCall(size_t i);
        void UpdateDrawCalls();
        virtual DragDropCompareFunc<Asset> GetDragDropAcceptor();

    private:
        Reference<Mesh> _mesh;
        Array<DrawCall> _drawCalls;
        Array<Reference<Material>> _materials;
    };

    class Animator : public Reflectable<Window*>
    {
        REFLECTABLE_PROTO("AMTR", Animator, Reflectable);

    public:
        explicit Animator(const Reference<Skeleton>&);

        void Get(const Array<glm::mat4>&, bool inverse = true) const;
        void SetSkeleton(const Reference<Skeleton>&);
        GET_CONST(const Reference<Skeleton>&, Skeleton, _skeleton);
        GET_SET(Reference<Animation>, Animation, _animation);
        GET_SET(float, Time, _time);

    protected:
        GET_CONST(const Array<TransformData>&, Transforms, _transforms);

    private:
        static bool DragDropAcceptor(const Reference<Asset>& asset, const Animator* animator);

        Reference<Skeleton> _skeleton;
        Reference<Animation> _animation;
        Array<TransformData> _transforms;
        float _time = DEFAULT;
    };

    class AnimatedMeshRenderer final : public MeshRenderer
    {
        REFLECTABLE_PROTO("AMRE", AnimatedMeshRenderer, MeshRenderer);

    public:
        AnimatedMeshRenderer(Entity* owner, Animator* animator, const Reference<Mesh>& mesh);

        void OnRender(float delta, Camera* camera) override;
        void OnUpdate(float delta) override;

        API::IVAO* GetVAO() const override;
        GPU::ObjectFlags GetFlags() const override;
        void SetMesh(const Reference<Mesh>& mesh) override;

    #ifdef GE_ENABLE_EDITOR
        GET_SET(bool, EnableDebugView, _enableDebugView);
    #endif

    protected:
        static bool DragDropAcceptor(const Reference<Asset>& asset, NoUserData);
        DragDropCompareFunc<Asset> GetDragDropAcceptor() override { return DragDropAcceptor; }
        static void AddPreviousPositionField(GPU::VAO& vao, size_t vertexCount);

    private:
        Pointer<API::IVAO> _vao = DEFAULT;
        RelativePointer<Animator> _animator = DEFAULT;

#ifdef GE_ENABLE_EDITOR
        bool _enableDebugView = false;
#endif
    };

#ifdef DEBUG
    GLOBAL GPU::VAO BoneDebugVAOFormat = []()
    {
        constexpr static glm::vec3 linePoints[2] { glm::vec3(0, 0, 0), glm::vec3(0, 0.1, 0) };

        GPU::VAO vao = DEFAULT;

        vao.PrimitiveType = GPU::PrimitiveType::Line;
        vao.AddMaterial(GPU::MaterialSlot("", 0, 1));
        vao.AddBuffer(GPU::Buffer(sizeof(glm::vec3) * 2, (const std::byte*) linePoints, sizeof(glm::vec3)));
        vao.AddField(GPU::VertexField("POS", GPU::ElementType::Float, false, 0, 0, 3, 0));

        return vao;
    }();
#endif

    class RendererManager : public ComponentManager<MeshRenderer>
    {
    public:
        explicit RendererManager(Window* window);

        void OnRender(float d, Camera* camera) override;

        GET(DrawCallManager&, DrawCallManager, _drawCallManager);
        GET_CONST(const API::ComputeShader&, SkinningShader, _skinningShader);
        GET_CONST(const API::Buffer<glm::mat4>&, Joints, _bonesBuffer);

#ifdef DEBUG
        GET_CONST(const API::VAO&, BoneDebugVAO, _boneDebugVAO);
        GET_CONST(const API::Shader&, BoneDebugShader, _boneDebugShader);
        GET_CONST(const API::Shader&, WireframeShader, _wireframeShader);
#endif

    private:
        DrawCallManager _drawCallManager;
        API::ComputeShader _skinningShader;
        API::Buffer<glm::mat4> _bonesBuffer;

#ifdef DEBUG
        API::VAO _boneDebugVAO;
        API::Shader _boneDebugShader;
        API::Shader _wireframeShader;
#endif
    };
}
