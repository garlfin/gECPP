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
        REFLECTABLE_PROTO("MREN", "gE::MeshRenderer", MeshRenderer, Component);

    public:
        MeshRenderer(Entity* o, const Reference<Mesh>& mesh);

        void OnInit() override {};
        void OnRender(float delta, Camera*) override {};

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

    private:
        Reference<Mesh> _mesh;
        Array<DrawCall> _drawCalls;
        Array<Reference<Material>> _materials;
    };

    class Animator : public Reflectable<Window*>
    {
        REFLECTABLE_PROTO("AMTR", "gE::Animator", Animator, Reflectable);

    public:
        explicit Animator(const Reference<Skeleton>&);

        GET_SET(Reference<Skeleton>, Skeleton, _skeleton);
        GET_SET(Reference<Animation>, Animation, _animation);

    private:
        Reference<Skeleton> _skeleton;
        Reference<Animation> _animation;
    };

    class AnimatedMeshRenderer : public MeshRenderer
    {
        REFLECTABLE_PROTO("AMRE", "gE::AnimatedMeshRenderer", AnimatedMeshRenderer, MeshRenderer);

    public:
        AnimatedMeshRenderer(Entity* owner, Animator* animator, const Reference<Mesh>& mesh);

        void OnRender(float delta, Camera* camera) override;
        API::IVAO& GetVAO() const override { return _vao; }
        void SetMesh(const Reference<Mesh>& mesh) override;

    private:
        Pointer<API::IVAO> _vao;
        RelativePointer<Animator> _animator;
    };

    class RendererManager : public ComponentManager<MeshRenderer>
    {
    public:
        explicit RendererManager(Window* window) : ComponentManager(window), _drawCallManager(window) {};

        void OnRender(float d, Camera* camera) override;

        GET(DrawCallManager&, DrawCallManager, _drawCallManager);
        GET(API::ComputeShader&, SkinningShader, _skinningShader);

    private:
        DrawCallManager _drawCallManager;
        API::ComputeShader _skinningShader;
    };
}
