//
// Created by scion on 11/4/2024.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Component/Physics/Collider.h>

namespace gE
{
    class PhysicsCubeEntity final : public Entity
    {
    public:
        PhysicsCubeEntity(Window* window, const Reference<GL::IVAO>& mesh, glm::vec3 size, EntityFlags flags = DEFAULT) :
            Entity(window, LayerMask::All, flags),
            _renderer(this, mesh),
            _rigidBody(this, RigidBodySettings(), _collider),
            _collider(this, Physics::BoxColliderSettings{ Physics::ConvexColliderSettings(), size })
        {
        }

        GET(MeshRenderer&, Renderer, _renderer);
        GET(RigidBody&, RigidBody, _rigidBody);
        GET(BoxCollider&, Collider, _collider);

    private:
        MeshRenderer _renderer;
        RigidBody _rigidBody;
        BoxCollider _collider;
    };
}
