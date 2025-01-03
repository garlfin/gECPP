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

    struct VertexWeight
    {
        glm::u8vec4 IDs = glm::u8vec4(-1);
        glm::u8vec4 Weights = glm::u8vec4(0);
    };

    struct Mesh : public Serializable<Window*>
    {
        SERIALIZABLE_PROTO(MESH, 2, Mesh, Serializable);

    public:
        std::string Name = DEFAULT;
        Array<std::string> MaterialNames = DEFAULT;

        AABB<Dimension::D3D> Bounds = DEFAULT;

        GET_CONST(const TypeSystem::Type*, MeshType, VAO->GetSettings().GetType());
        SmartPointer<API::IVAO> VAO;

        GET_CONST(const TypeSystem::Type*, ShapeType, Shape->GetSettings().GetType());
        SmartPointer<Jolt::Shape> Shape;

        SmartPointer<API::Buffer<VertexWeight>> BoneWeights = DEFAULT;
    };
}
