//
// Created by scion on 11/3/2024.
//

#pragma once

#include "Physics.h"
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <Engine/Component/Component.h>
#include <Engine/Math/Math.h>

namespace gE
{
    struct ColliderTransform
    {
        glm::vec3 Position = glm::vec3(0.0);
        glm::quat Rotation = glm::identity<glm::quat>();
    };

    struct ColliderSettings
    {
        ColliderTransform Transform;
    };

    struct ConvexColliderSettings : public ColliderSettings
    {
        float Mass = 1.0;
    };

    struct SphereColliderSettings : public ConvexColliderSettings
    {
        float Radius = 1.f;
    };

    struct BoxColliderSettings : public ConvexColliderSettings
    {
        glm::vec3 Extents = glm::vec3(1.f);
    };

    class Collider : public Component
    {
    public:
        Collider(Entity* owner, const ColliderSettings& settings, px::Shape& shape) : Component(owner),
            _settings(settings), _shape(shape)
        {

        }

        GET_CONST(const ColliderTransform&, Transform, _settings.Transform);
        GET_CONST(const px::Shape&, Shape, _shape);

    private:
        ColliderSettings _settings;
        RelativePointer<px::Shape> _shape;
    };

    class ConvexCollider : public Collider
    {
    public:
        ConvexCollider(Entity* owner, const ConvexColliderSettings& settings, px::ConvexShape& shape) :
            Collider(owner, settings, shape)
        {
        }

        GET_CONST(float, Density, GetShape().GetDensity());
        GET_CONST(float, Mass, GetShape().GetDensity() * GetShape().GetVolume());

        inline void SetDensity(float density) { GetShape().SetDensity(density); }
        inline void SetMass(float mass) { GetShape().SetDensity(mass * GetShape().GetVolume()); }

        GET_CONST(const px::ConvexShape&, Shape, (px::ConvexShape&) Collider::GetShape());

    protected:
        NODISCARD ALWAYS_INLINE px::ConvexShape& GetShape() { return (px::ConvexShape&) Collider::GetShape(); }
    };

    class SphereCollider final : public ConvexCollider
    {
    public:
        SphereCollider(Entity*, const SphereColliderSettings& s);

        GET_CONST(float, Radius, _radius);
        GET_CONST(const px::SphereShape&, Shape, *_shape);

    private:
        float _radius;
        ManagedPX<px::SphereShape> _shape;
    };

    class BoxCollider final : public ConvexCollider
    {
    public:
        BoxCollider(Entity*, const BoxColliderSettings& s);

        GET_CONST(glm::vec3, Scale, _scale);
        GET_CONST(const px::BoxShape&, Shape, *_shape);

    private:
        glm::vec3 _scale;
        ManagedPX<px::BoxShape> _shape;
    };
}
