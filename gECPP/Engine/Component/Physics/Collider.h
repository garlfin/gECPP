//
// Created by scion on 11/3/2024.
//

#pragma once

#include <Physics/Shapes.h>
#include <Engine/Component/Component.h>
#include <Engine/Entity/Entity.h>

namespace gE
{
    class Collider : public Component
    {
    public:
        Collider(Entity* owner, Jolt::Shape& shape) :
            Component(owner),
            _shape(shape)
        {

        }

        const Jolt::Shape* operator->() { return _shape.Get(); }

        GET_CONST(const Physics::ColliderTransform&, Transform, GetSettings().Transform);
        GET_CONST(const Physics::Shape&, Settings, _shape->GetSettings());
        GET_CONST(const Jolt::Shape&, Shape, *_shape);

    private:
        RelativePointer<Jolt::Shape> _shape;
    };

    class ConvexCollider : public Collider
    {
    public:
        ConvexCollider(Entity* owner, Jolt::ConvexShape& shape) :
            Collider(owner, shape)
        {
        }

        GET_CONST(float, Density, GetShape().GetDensity());
        GET_CONST(float, Mass, GetShape().GetDensity() * GetShape().GetVolume());

        inline void SetDensity(float density) { GetShape().SetDensity(density); }
        inline void SetMass(float mass) { GetShape().SetDensity(mass * GetShape().GetVolume()); }

        const Jolt::ConvexShape* operator->() { return &GetShape(); }

        GET_CONST(const Physics::ConvexShape&, Settings, GetShape().GetSettings());
        GET_CONST(const Jolt::ConvexShape&, Shape, (const Jolt::ConvexShape&) Collider::GetShape());

    protected:
        NODISCARD ALWAYS_INLINE Jolt::ConvexShape& GetShape() { return (Jolt::ConvexShape&) Collider::GetShape(); }
    };

    template<class SHAPE_T, class SUPER_T>
    class ShapeCollider final : public SUPER_T
    {
    public:
        ShapeCollider(Entity* owner, const typename SHAPE_T::SUPER& settings) :
            SUPER_T(owner, _shape),
            _shape(&owner->GetWindow(), settings)
        {
            _shape.Free();
        }

        ShapeCollider(Entity* owner, const SHAPE_T& shape) :
            SUPER_T(owner, _shape),
            _shape(shape)
        {
            if(!shape.IsFree()) LOG("WARNING: SHAPE NOT FREED BEFORE BEING ASSIGNED TO COLLIDER!");
        }

        ShapeCollider(Entity* owner, SHAPE_T&& shape) :
            SUPER_T(owner, _shape),
            _shape(std::move(shape))
        {
            if(!shape.IsFree()) LOG("WARNING: SHAPE NOT FREED BEFORE BEING ASSIGNED TO COLLIDER!");
        }

        const SHAPE_T* operator->() { return &_shape; }

        GET_CONST(const typename SHAPE_T::SUPER&, Settings, _shape->GetSettings());
        GET_CONST(const SHAPE_T&, Shape, _shape);

    protected:
        NODISCARD ALWAYS_INLINE SHAPE_T& GetShape() { return _shape; }

    private:
        SHAPE_T _shape;
    };

    using SphereCollider = ShapeCollider<Jolt::SphereShape, ConvexCollider>;
    using BoxCollider = ShapeCollider<Jolt::BoxShape, ConvexCollider>;
}
