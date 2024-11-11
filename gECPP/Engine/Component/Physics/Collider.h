//
// Created by scion on 11/3/2024.
//

#pragma once

#include <Physics/Physics.h>
#include <Physics/Shapes.h>

#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <Engine/Component/Component.h>
#include <Engine/Math/Math.h>

namespace gE
{
    class Collider : public Component
    {
    public:
        Collider(Entity* owner, const Physics::Shape& settings, px::Shape& shape) :
            Component(owner),
            _settings(settings), _shape(shape)
        {

        }

        const Physics::Shape* operator->() { return _shape.Get(); }

        GET_CONST(const Physics::ColliderTransform&, Transform, _settings.Transform);
        GET_CONST(const Physics::Shape&, Shape, _shape);

    private:
        Physics::ColliderSettings _settings;
        RelativePointer<Physics::Shape> _shape;
    };

    class ConvexCollider : public Collider
    {
    public:
        ConvexCollider(Entity* owner, const Physics::ConvexColliderSettings& settings, px::ConvexShape& shape) :
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
        SphereCollider(Entity*, const Physics::SphereColliderSettings& s);

        GET_CONST(float, Radius, _radius);
        GET_CONST(const px::SphereShape&, Shape, *_shape);

    private:
        float _radius;
        ManagedPX<px::SphereShape> _shape;
    };

    class BoxCollider final : public ConvexCollider
    {
    public:
        BoxCollider(Entity*, const Physics::BoxShape& s);

        GET_CONST(glm::vec3, Scale, _scale);
        GET_CONST(const px::BoxShape&, Shape, *_shape);

    private:
        Jolt::BoxShape _shape;
    };
}
