//
// Created by scion on 11/9/2024.
//

#pragma once

#include "Serializable.h"
#include <Graphics/Buffer/VAO.h>
#include <Engine/Math/Collision/AABB.h>

namespace gE
{
    enum VAOType : u8
    {
        VAO,
        IndexedVAO
    };

    struct Mesh : public Serializable<Window*>
    {
        SERIALIZABLE_PROTO(MESH, 1, Mesh, Serializable);

    public:
        std::string Name;
        Array<std::string> MaterialNames;

        AABB<Dimension::D3D> AABB;

        VAOType Type;
        API::IVAO* VAO;
    };
}
