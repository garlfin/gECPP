//
// Created by scion on 3/5/2025.
//

#include "MeshLoader.h"

#include <Vendor/FastGLTF/core.hpp>

#include "Vertex.h"

CONSTEXPR_GLOBAL fastgltf::Options GLTF_LOAD_OPTIONS = fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices;

namespace gE::Model
{
    struct PrimitiveData
    {
        const fastgltf::Attribute* Attribute = nullptr;
        const fastgltf::Accessor* Accessor = nullptr;
        const fastgltf::BufferView* BufferView = nullptr;
        const fastgltf::Buffer* Buffer = nullptr;
    };

    PrimitiveData GetAttributeData(const fastgltf::Asset& file, const fastgltf::Primitive& prim, std::string_view attribute);
    PrimitiveData GetIndicesData(const fastgltf::Asset& file, const fastgltf::Primitive& prim);

    void SetupMesh(GPU::VAO& meshOut, size_t submeshCount, size_t vertexCount);
    void ProcessSubmesh(GPU::VAO& meshOut, const fastgltf::Asset& file, const fastgltf::Mesh& meshIn);
    void ProcessSubmesh(GPU::IndexedVAO& meshOut, const fastgltf::Asset& file, const fastgltf::Mesh& meshIn);

    void ReadGLTF(Window* window, const Path& path, Array<Mesh>& meshesOut, Array<Skeleton>& skeletons)
    {
        fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath(path);
        if(data.error() != fastgltf::Error::None)
            return Log::Error(std::format("Failed to open {}.", path));

        fastgltf::Parser parser;
        fastgltf::Expected<fastgltf::Asset> file = parser.loadGltf(data.get(), path.parent_path(), GLTF_LOAD_OPTIONS);
        if(data.error() != fastgltf::Error::None)
            return Log::Error(std::format("Failed to parse {}.", path));

        meshesOut = Array<Mesh>(file->meshes.size());
        for(size_t i = 0; i < file->meshes.size(); i++)
        {
            const fastgltf::Mesh& meshIn = file->meshes[i];
            Mesh& meshOut = meshesOut[i];

            meshOut.Name = meshIn.name;

            size_t vertexCount = 0;
            size_t indexCount = 0;
            for(const fastgltf::Primitive& subMesh : meshIn.primitives)
            {
                const PrimitiveData position = GetAttributeData(file.get(), subMesh, "POSITION");
                const PrimitiveData indices = GetIndicesData(file.get(), subMesh);

                GE_ASSERT(position.Accessor);
                vertexCount += position.Accessor->count;

                if(!indices.Accessor) return;

            #ifdef DEBUG
                if(indexCount) GE_ASSERT(indices.Accessor->count);
            #endif

                indexCount += indices.Accessor->count;
            }

            if(indexCount)
            {
                GPU::IndexedVAO meshSettings = DEFAULT;

                SetupMesh(meshSettings, meshIn.primitives.size(), vertexCount);
                ProcessSubmesh(meshSettings, file.get(), meshIn);

                meshOut.VAO = ptr_create<API::IndexedVAO>(window, move(meshSettings));
            }
            else
            {
                GPU::VAO meshSettings = DEFAULT;

                SetupMesh(meshSettings, meshIn.primitives.size(), vertexCount);
                ProcessSubmesh(meshSettings, file.get(), meshIn);

                meshOut.VAO = ptr_create<API::VAO>(window, move(meshSettings));
            }
        }
    }

    PrimitiveData GetAttributeData(const fastgltf::Asset& file, const fastgltf::Primitive& prim, std::string_view attribute)
    {
        PrimitiveData data = DEFAULT;

        data.Attribute = prim.findAttribute(attribute);
        if(!data.Attribute) return data;

        data.Accessor = &file.accessors[data.Attribute->accessorIndex];

        if(data.Accessor->bufferViewIndex.has_value())
            data.BufferView = &file.bufferViews[data.Accessor->bufferViewIndex.value()];
        else
            return data;

        data.Buffer = &file.buffers[data.BufferView->bufferIndex];

        return data;
    }

    PrimitiveData GetIndicesData(const fastgltf::Asset& file, const fastgltf::Primitive& prim)
    {
        PrimitiveData data = DEFAULT;

        if(!prim.indicesAccessor.has_value())
            return data;

        data.Accessor = &file.accessors[prim.indicesAccessor.value()];

        if(data.Accessor->bufferViewIndex.has_value())
            data.BufferView = &file.bufferViews[data.Accessor->bufferViewIndex.value()];
        else
            return data;

        data.Buffer = &file.buffers[data.BufferView->bufferIndex];

        return data;
    }

    void SetupMesh(GPU::VAO& meshOut, size_t submeshCount, size_t vertexCount)
    {
        meshOut.Counts.BufferCount = 1;
        meshOut.Counts.FieldCount = 5;
        meshOut.Counts.MaterialCount = submeshCount;

        GPU::Buffer<>& buf = meshOut.Buffers[0];
        buf.Stride = sizeof(Vertex);
        buf.Data = Array<u8>(vertexCount * buf.Stride);
        buf.UsageHint = GPU::BufferUsageHint::Default;
    }

    void ProcessSubmesh(GPU::VAO& meshOut, const fastgltf::Asset& file, const fastgltf::Mesh& meshIn)
    {
        size_t offset = 0;
        for(const fastgltf::Primitive& submeshIn : meshIn.primitives)
        {
            GE_ASSERT(submeshIn.type == fastgltf::PrimitiveType::Triangles)

            const PrimitiveData position = GetAttributeData(file, submeshIn, "POSITION");
            const PrimitiveData uv = GetAttributeData(file, submeshIn, "UV0");
            const PrimitiveData normal = GetAttributeData(file, submeshIn, "NORMAL");
            const PrimitiveData tangent = GetAttributeData(file, submeshIn, "TANGENT");
            const PrimitiveData indices = GetIndicesData(file, submeshIn);

            for(size_t i = 0; i < position.Accessor->count; i++)
            {

            }

            offset += position.Accessor->count;
        }
    }

    void ProcessSubmesh(GPU::IndexedVAO& meshOut, const fastgltf::Asset& file, const fastgltf::Mesh& meshIn)
    {
        ProcessSubmesh((GPU::VAO&) meshOut, meshIn);
    }
}
