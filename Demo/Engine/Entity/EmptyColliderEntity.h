//
// Created by scion on 11/5/2024.
//

#pragma once

#include <Component/Physics/Collider.h>
#include <Entity/Entity.h>

namespace gE
{
    inline Physics::BoxShape GetShapeSettings(const glm::vec3& extents);

    class EmptyColliderEntity : public Entity
    {
    public:
        EmptyColliderEntity(Window* window, glm::vec3 size, EntityFlags flags = DEFAULT) :
            Entity(window, nullptr, LayerMask::All, flags),
            _rigidBody(this, RigidBodySettings(), _collider),
            _collider(this, GetShapeSettings(size))
        {
        }

        GET(RigidBody&, RigidBody, _rigidBody);
        GET(BoxCollider&, Collider, _collider);

    private:
        RigidBody _rigidBody;
        BoxCollider _collider;
    };

    inline Physics::BoxShape GetShapeSettings(const glm::vec3& extents)
    {
        Physics::BoxShape shape = DEFAULT;
        shape.Extents = extents;

        return shape;
    }
}
