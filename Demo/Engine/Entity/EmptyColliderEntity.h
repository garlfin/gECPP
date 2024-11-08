//
// Created by scion on 11/5/2024.
//

#pragma once

#include <Engine/Entity/Entity.h>
#include <Engine/Component/Physics/Collider.h>

namespace gE
{
    class EmptyColliderEntity : public Entity
    {
    public:
        EmptyColliderEntity(Window* window, glm::vec3 size, EntityFlags flags = DEFAULT) :
            Entity(window, LayerMask::All, flags),
            _rigidBody(this, RigidBodySettings(), _collider),
            _collider(this, BoxColliderSettings{ ConvexColliderSettings(), size })
        {
        }

        GET(RigidBody&, RigidBody, _rigidBody);
        GET(BoxCollider&, Collider, _collider);

    private:
        RigidBody _rigidBody;
        BoxCollider _collider;
    };
}
