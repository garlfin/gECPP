//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Engine/Math/Collision/AABB.h>
#include <Engine/Utility/AssetManager.h>
#include <Graphics/Buffer/VAO.h>
#include <Physics/Shapes.h>
#include <Serializable/Serializable.h>

namespace gE
{
    enum class ShapeType : u8
    {
        None,
        Box,
        Sphere,
        Cylinder,
        Capsule,
        Compound,
        ConvexMesh,
        Mesh
    };

    struct Mesh : public Serializable<Window*>
    {
        SERIALIZABLE_PROTO(MESH, 1, Mesh, Serializable);

    public:
        std::string Name;
        Array<std::string> MaterialNames;

        AABB<Dimension::D3D> AABB;

        GET_CONST(const TypeSystem::Type*, MeshType, VAO->GetData().GetType());
        SmartPointer<API::IVAO> VAO;

        // GET_CONST(const TypeSystem::Type*, ShapeType, VAO->GetData().GetType());
        SmartPointer<Jolt::Shape> Shape;
    };
}
