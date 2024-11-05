//
// Created by scion on 11/3/2024.
//

#pragma once

#include "Physics.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace gE
{
    struct SphereColliderSettings : public RigidBodySettings
    {
        float Radius = 1.f;
    };

    class SphereCollider : public RigidBody
    {
    public:
        SphereCollider(Entity*, const SphereColliderSettings& s);

        GET_CONST(float, Radius, _radius);

    private:
        float _radius;
        ManagedPX<px::SphereShape> _shape;
    };

    struct BoxColliderSettings : public RigidBodySettings
    {
        glm::vec3 Extents = glm::vec3(1.f);
    };

    class BoxCollider : public RigidBody
    {
    public:
        BoxCollider(Entity*, const BoxColliderSettings& s);

        GET_CONST(glm::vec3, Scale, _scale);

    private:
        glm::vec3 _scale;
        ManagedPX<px::BoxShape> _shape;
    };
}
