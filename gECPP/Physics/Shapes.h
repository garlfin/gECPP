//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Serializable/Serializable.h>

#include "Physics.h"
#include <Engine/Utility/RelativePointer.h>
#include <Serializable/Macro.h>
#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>

namespace Physics
{
    struct ColliderTransform
    {
        glm::vec3 Position = glm::vec3(0.0);
        glm::quat Rotation = glm::identity<glm::quat>();
    };

    struct Shape : public Serializable<gE::Window*>, public gE::Asset
    {
        SERIALIZABLE_PROTO(SHPE, 1, Shape, Serializable);

    public:
        void Free() override {};
        NODISCARD bool IsFree() const override { return true; };

        ColliderTransform Transform = DEFAULT;
    };

    struct ConvexShape : public Shape
    {
        SERIALIZABLE_PROTO(CSHP, 1, ConvexShape, Shape);

    public:
        bool operator==(const ConvexShape& o) const { return Mass == o.Mass; }

        float Mass = 1.0;
    };

    struct SphereShape : public ConvexShape
    {
        SERIALIZABLE_PROTO(SSHP, 1, SphereShape, ConvexShape);

    public:
        bool operator==(const SphereShape& o) const
        {
            return ConvexShape::operator==(o) && Radius == o.Radius;
        }

        float Radius = 1.f;
    };

    struct BoxShape : public ConvexShape
    {
        SERIALIZABLE_PROTO(BSHP, 1, BoxShape, ConvexShape);

    public:
        bool operator==(const BoxShape& o) const
        {
            return ConvexShape::operator==(o) && Extents == o.Extents;
        }

        glm::vec3 Extents = glm::vec3(1.f);
    };

    struct CapsuleShape : public ConvexShape
    {
        SERIALIZABLE_PROTO(CPSL, 1, CapsuleShape, ConvexShape);

    public:
        bool operator==(const CapsuleShape& o) const
        {
            return ConvexShape::operator==(o) && Height == o.Height && Radius == o.Radius;
        }

        float Height = 1.75f;
        float Radius = 0.225f;
    };
}

namespace Jolt
{
    class Shape
    {
    public:
        Shape() = default;
        Shape(Physics::Shape& settings, gE::ManagedPX<px::Shape>& shape);

        GET_CONST(const Physics::ColliderTransform&, Transform, _settings->Transform);
        GET_CONST(const Physics::Shape&, Settings, _settings);
        GET(px::Shape&, JoltShape, **_shape);

        NODISCARD operator bool() const { return _shape.GetPointer() && _shape->GetPointer(); }

    private:
        RelativePointer<Physics::Shape> _settings = DEFAULT;
        RelativePointer<gE::ManagedPX<px::Shape>> _shape = DEFAULT;
    };

    class ConvexShape : public Shape
    {
    public:
        ConvexShape() = default;
        ConvexShape(Physics::ConvexShape& settings, gE::ManagedPX<px::ConvexShape>& shape);

        GET_CONST(float, Density, GetJoltShape().GetDensity());
        GET_CONST(float, Volume, GetJoltShape().GetVolume());
        GET_CONST(float, Mass, GetDensity() * GetVolume());

        inline void SetDensity(float density) { GetJoltShape().SetDensity(density); }
        inline void SetMass(float mass) { GetJoltShape().SetDensity(mass * GetJoltShape().GetVolume()); }

        GET_CONST(const Physics::ConvexShape&, Settings, (const Physics::ConvexShape&) Shape::GetSettings());
        GET(px::ConvexShape&, JoltShape, (px::ConvexShape&) Shape::GetJoltShape());
    };

    class SphereShape : protected Physics::SphereShape, public ConvexShape
    {
        API_SERIALIZABLE(SphereShape, Physics::SphereShape);

    public:
        GET(px::SphereShape&, Shape, *_shape);

    private:
        gE::ManagedPX<px::SphereShape> _shape;
    };

    class BoxShape : protected Physics::BoxShape, public ConvexShape
    {
        API_SERIALIZABLE(BoxShape, Physics::BoxShape);

    public:
        GET(px::BoxShape&, JoltShape, *_shape);

    private:
        gE::ManagedPX<px::BoxShape> _shape;
    };

    class CapsuleShape : protected Physics::CapsuleShape, public ConvexShape
    {
        API_SERIALIZABLE(CapsuleShape, Physics::CapsuleShape);
        API_DEFAULT_CM_CONSTRUCTOR(CapsuleShape);

    public:
        GET(px::CapsuleShape&, JoltShape, *_shape);

    private:
        gE::ManagedPX<px::CapsuleShape> _shape;
    };
}

#include "Shapes.inl"