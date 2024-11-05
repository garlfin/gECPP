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
        EmptyColliderEntity(Window* window, glm::vec3 size, Flags flags = DEFAULT) :
            Entity(window, LayerMask::All, flags),
            _collider(this, BoxColliderSettings{ RigidBodySettings(), size })
        {
        }

        GET(BoxCollider&, Collider, _collider);

    private:
        BoxCollider _collider;
    };
}
