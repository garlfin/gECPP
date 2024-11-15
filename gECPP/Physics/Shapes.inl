//
// Created by scion on 11/9/2024.
//

#pragma once

#include "Shapes.h"

namespace Physics
{
    inline void Shape::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Transform);
    }

    inline void Shape::IDeserialize(ostream& out) const
    {
        Write(out, Transform);
    }

    inline void ConvexShape::ISerialize(istream& in, SETTINGS_T s)
    {
        Mass = Read<float>(in);
    }

    inline void ConvexShape::IDeserialize(ostream& out) const
    {
        Write(out, Mass);
    }

    inline void SphereShape::ISerialize(istream& in, SETTINGS_T s)
    {
        Radius = Read<float>(in);
    }

    inline void SphereShape::IDeserialize(ostream& out) const
    {
        Write(out, Radius);
    }

    inline void BoxShape::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Extents);
    }

    inline void BoxShape::IDeserialize(ostream& out) const
    {
        Write(out, Extents);
    }
}

namespace Jolt
{
    inline Shape::Shape(Physics::Shape& settings, px::Shape& shape) :
        _settings(&settings), _shape(shape)
    {
    }

    inline ConvexShape::ConvexShape(Physics::ConvexShape& settings, px::ConvexShape& shape) :
        Shape(settings, shape)
    {
    }
}
