//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Core/Material/Material.h>
#include <Core/Mesh/Mesh.h>
#include <Entity/Entity.h>
#include <Graphics/Buffer/VAO.h>

#include "DrawCall.h"

namespace gE
{
    class MeshRenderer : public Component
    {
        REFLECTABLE_PROTO("gE::MeshRenderer", "MREN", MeshRenderer, Component);

    public:
        MeshRenderer(Entity* o, const Reference<Mesh>& mesh);

        void OnInit() override {};
        void OnUpdate(float delta) override {};

        GET_CONST(const Reference<Mesh>&, Mesh, _mesh);
        virtual void SetMesh(const Reference<Mesh>& mesh);

        NODISCARD ALWAYS_INLINE Material* GetMaterial(u8 i) const { return _materials[i].GetPointer(); }

        void SetMaterial(u8 i, const Reference<Material>& mat);
        void SetMaterial(u8 i, Reference<Material>&& mat);
        void SetNullMaterial(u8 i);

        virtual API::IVAO& GetVAO() const { return _mesh->VAO; }

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
        REFLECTABLE_PROTO("gE::Animator", "AMTR", Animator, Reflectable);

    public:
        explicit Animator(const Reference<Skeleton>&);

        void Get(const Array<glm::mat4>&) const;
        void SetSkeleton(const Reference<Skeleton>&);
        GET_CONST(const Reference<Skeleton>&, Skeleton, _skeleton);
        GET_SET(Reference<Animation>, Animation, _animation);

    protected:
        GET_CONST(const Array<TransformData>&, Transforms, _transforms);

    private:
        static bool DragDropAcceptor(const Reference<Asset>& asset, const Animator* animator);

        Reference<Skeleton> _skeleton;
        Reference<Animation> _animation;
        Array<TransformData> _transforms;
        float _time;
    };

    class AnimatedMeshRenderer final : public MeshRenderer
    {
        REFLECTABLE_PROTO("gE::AnimatedMeshRenderer", "AMRE", AnimatedMeshRenderer, MeshRenderer);

    public:
        AnimatedMeshRenderer(Entity* owner, Animator* animator, const Reference<Mesh>& mesh);

        void OnUpdate(float delta) override;
        API::IVAO& GetVAO() const override;
        void SetMesh(const Reference<Mesh>& mesh) override;

    protected:
        static bool DragDropAcceptor(const Reference<Asset>& asset, NoUserData);
        DragDropCompareFunc<Asset> GetDragDropAcceptor() override { return DragDropAcceptor; }

    private:
        Pointer<API::IVAO> _vao;
        RelativePointer<Animator> _animator;
    };

    class RendererManager : public ComponentManager<MeshRenderer>
    {
    public:
        explicit RendererManager(Window* window);;

        void OnRender(float d, Camera* camera) override;

        GET(DrawCallManager&, DrawCallManager, _drawCallManager);
        GET(API::ComputeShader&, SkinningShader, _skinningShader);
        GET(API::Buffer<glm::mat4>&, Joints, _bonesBuffer);

    private:
        DrawCallManager _drawCallManager;
        API::ComputeShader _skinningShader;
        API::Buffer<glm::mat4> _bonesBuffer;
    };
}
