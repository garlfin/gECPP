//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Math/Collision/AABB.h>
#include <Core/Serializable/Serializable.h>
#include <Graphics/Buffer/VAO.h>
#include <Physics/Shapes.h>

#include "Skeleton.h"

namespace gE
{
    struct Mesh : public Asset
    {
        SERIALIZABLE_PROTO("gE::Mesh", "MESH", 2, Mesh, Asset);
        REFLECTABLE_NAME_PROTO();

    public:
        void Free() override;
        NODISCARD bool IsFree() const override;

        std::string Name = DEFAULT;
        Array<std::string> MaterialNames = DEFAULT;

        AABB<Dimension::D3D> Bounds = DEFAULT;

        GET_CONST(const TYPE_T*, MeshType, VAO->GetSettings().GetType());
        Pointer<API::IVAO> VAO;

        GET_CONST(const TYPE_T*, ShapeType, Shape->GetSettings().GetType());
        Pointer<Jolt::Shape> Shape;

        Reference<Skeleton> Skeleton;
    };
}
