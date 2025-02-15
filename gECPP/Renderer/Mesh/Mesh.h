//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Math/Collision/AABB.h>
#include <Utility/AssetManager.h>
#include <Graphics/Buffer/VAO.h>
#include <Physics/Shapes.h>
#include <Serializable/Serializable.h>

namespace gE
{
    struct VertexWeight
    {
        glm::u8vec4 IDs = glm::u8vec4(-1);
        glm::u8vec4 Weights = glm::u8vec4(0);
    };

    struct Mesh : public Asset
    {
        SERIALIZABLE_PROTO("MESH", 2, Mesh, Asset);

    public:
        void Free() override;
        NODISCARD bool IsFree() const override;

        std::string Name = DEFAULT;
        Array<std::string> MaterialNames = DEFAULT;

        AABB<Dimension::D3D> Bounds = DEFAULT;

        GET_CONST(const Type<gE::Window*>*, MeshType, VAO->GetSettings().GetType());
        Pointer<API::IVAO> VAO;

        GET_CONST(const Type<gE::Window*>*, ShapeType, Shape->GetSettings().GetType());
        Pointer<Jolt::Shape> Shape;

        Pointer<API::Buffer<VertexWeight>> BoneWeights = DEFAULT;
    };
}
