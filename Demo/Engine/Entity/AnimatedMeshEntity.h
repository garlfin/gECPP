//
// Created by scion on 3/21/2025.
//

#pragma once

#include <Component/MeshRenderer/MeshRenderer.h>
#include <Entity/Entity.h>

namespace gE
{
    class AnimatedMeshEntity : public Entity
    {
        REFLECTABLE_PROTO("STME", AnimatedMeshEntity, Entity);

    public:
        AnimatedMeshEntity(Window* window, const Reference<Mesh>& mesh, Entity* parent = nullptr) : Entity(window, parent),
            _animator(mesh->Skeleton),
            _renderer(this, &_animator, mesh)
        {};

        GET(gE::MeshRenderer&, Renderer, _renderer);

    private:
        Animator _animator;
        AnimatedMeshRenderer _renderer;
    };

    REFLECTABLE_FACTORY_NO_IMPL(AnimatedMeshEntity, inline);
    inline REFLECTABLE_ONGUI_IMPL(AnimatedMeshEntity,
    {
        DrawField(Field{ "Renderer"sv }, _renderer, depth);
        DrawField(Field{ "Animator"sv }, _animator, depth);
    });
}
