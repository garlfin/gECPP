//
// Created by scion on 11/4/2024.
//

#pragma once

#include <Entity/Entity.h>
#include <Component/Physics/Collider.h>

namespace gE
{
    inline Physics::BoxShape GetShapeSettings(const glm::vec3& extents);

    class PhysicsCubeEntity final : public Entity
    {
    public:
        PhysicsCubeEntity(Window* window, const Reference<Mesh>& mesh, glm::vec3 size, EntityFlags flags = DEFAULT) :
            Entity(window, nullptr, LayerMask::All, flags),
            _renderer(this, mesh),
            _rigidBody(this, RigidBodySettings(), _collider),
            _collider(this, GetShapeSettings(size))
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
