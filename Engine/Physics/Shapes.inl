//
// Created by scion on 11/9/2024.
//

#pragma once

#include "Shapes.h"

namespace Physics
{
    inline void Shape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Transform);
    }

    inline void Shape::ISerialize(ostream& out) const
    {
        Write(out, Transform);
    }

    inline void ConvexShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Mass = Read<float>(in);
    }

    inline void ConvexShape::ISerialize(ostream& out) const
    {
        Write(out, Mass);
    }

    inline void SphereShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Radius = Read<float>(in);
    }

    inline void SphereShape::ISerialize(ostream& out) const
    {
        Write(out, Radius);
    }

    inline void BoxShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Extents);
    }

    inline void BoxShape::ISerialize(ostream& out) const
    {
        Write(out, Extents);
    }

    inline void CapsuleShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Height);
        Read(in, Radius);
    }

    inline void CapsuleShape::ISerialize(ostream& out) const
    {
        Write(out, Height);
        Write(out, Radius);
    }

    inline void BakedConvexMeshShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, CenterOfMass);
        Read(in, Inertia);
        Read(in, Bounds);
        ReadArray<u16>(in, Points);
        ReadArray<u16>(in, Faces);
        ReadArray<u16>(in, VertexIDs);
        Read(in, ConvexRadius);
        Read(in, Volume);
        Read(in, InnerRadius);
    }

    inline void BakedConvexMeshShape::ISerialize(ostream& out) const
    {
        Write(out, CenterOfMass);
        Write(out, Inertia);
        Write(out, Bounds);
        WriteArray<u16>(out, Points);
        WriteArray<u16>(out, Faces);
        WriteArray<u16>(out, VertexIDs);
        Write(out, ConvexRadius);
        Write(out, Volume);
        Write(out, InnerRadius);
    }

    inline void ConvexMeshShape::IDeserialize(istream& in, SETTINGS_T s)
    {
        ReadArray<u32>(in, Points);

        // HasBakedSettings
        if(!Read<bool>(in)) return;

        BakedSettings = ptr_create<BakedConvexMeshShape>(in, nullptr);
    }

    inline void ConvexMeshShape::ISerialize(ostream& out) const
    {
        const bool hasBakedSettings = !BakedSettings.IsFree();

        WriteArray<u32>(out, Points);
        Write(out, hasBakedSettings);

        if(hasBakedSettings)
            Write(out, BakedSettings);
    }
}

namespace Jolt
{
    inline Shape::Shape(Physics::Shape& settings, gE::ManagedPX<px::Shape>& shape) :
        _settings(&settings), _shape(&shape)
    {
    }

    inline ConvexShape::ConvexShape(Physics::ConvexShape& settings, gE::ManagedPX<px::ConvexShape>& shape) :
        Shape(settings, shape.To<px::Shape>())
    {
    }
}
