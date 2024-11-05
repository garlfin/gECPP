//
// Created by scion on 11/4/2024.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Component/Physics/Collider.h>

namespace gE
{
    class PhysicsCubeEntity : public Entity
    {
    public:
        PhysicsCubeEntity(Window* window, const Reference<GL::IVAO>& mesh) : Entity(window),
            _renderer(this, mesh),
            _collider(this, BoxColliderSettings())
        {
        }

        GET(MeshRenderer&, Renderer, _renderer);
        GET(BoxCollider&, Collider, _collider);

    private:
        MeshRenderer _renderer;
        BoxCollider _collider;
    };
}
