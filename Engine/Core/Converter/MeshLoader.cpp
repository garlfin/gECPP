//
// Created by scion on 3/5/2025.
//

#include "MeshLoader.h"
#include "MeshUtility.h"

#include <Vendor/FastGLTF/core.hpp>

CONSTEXPR_GLOBAL gltf::Options GLTF_LOAD_OPTIONS = gltf::Options::LoadExternalBuffers | gltf::Options::GenerateMeshIndices;

#define GE_MODEL_CHECK_ATTR(FIELD, PRIM_DATA, ...) \
    if(PRIM_DATA) \
        FillBuffer(FIELD, writeSpan, PRIM_DATA, __VA_ARGS__); \
    else \
        Log::Write(std::format("Warning: Failed to find field \"{}\" in mesh {}.\n", #FIELD, meshIn.name));

namespace gE::Model
{
    PrimitiveData GetAttributeData(const gltf::Asset& file, const gltf::Primitive& prim, std::string_view attribute);
    PrimitiveData GetIndicesData(const gltf::Asset& file, const gltf::Primitive& prim);

    void SetupMeshFields(GPU::VAO& meshOut, size_t vertexCount);
    void SetupMeshFields(GPU::IndexedVAO& meshOut, size_t vertexCount, size_t triCount);
    void SetupMeshMaterials(GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& mesh);
    void ProcessSubmesh(GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn);
    void ProcessSubmesh(GPU::IndexedVAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn);

    TransformData GetTransformFromNode(const gltf::Node& node);

    void ReadGLTF(Window* window, const Path& path, Array<Mesh>& meshesOut, Array<Skeleton>& skeletons)
    {
        gltf::Expected<gltf::GltfDataBuffer> data = gltf::GltfDataBuffer::FromPath(path);
        if(data.error() != gltf::Error::None)
            return Log::Error(std::format("Failed to open {}.", path.string()));

        gltf::Parser parser;
        gltf::Expected<gltf::Asset> file = parser.loadGltf(data.get(), path.parent_path(), GLTF_LOAD_OPTIONS);
        if(data.error() != gltf::Error::None)
            return Log::Error(std::format("Failed to parse {}.", path.string()));

        meshesOut = Array<Mesh>(file->meshes.size());
        for(size_t i = 0; i < file->meshes.size(); i++)
        {
            const gltf::Mesh& meshIn = file->meshes[i];
            Mesh& meshOut = meshesOut[i];

            meshOut.Name = meshIn.name;

            size_t vertexCount = 0;
            size_t indexCount = 0;
            for(const gltf::Primitive& subMesh : meshIn.primitives)
            {
                const PrimitiveData position = GetAttributeData(file.get(), subMesh, "POSITION");
                const PrimitiveData indices = GetIndicesData(file.get(), subMesh);

                GE_ASSERT(position);
                vertexCount += position.Accessor->count;

                if(!indices.Accessor) return;

            #ifdef DEBUG
                if(indexCount) GE_ASSERT(indices.Accessor->count);
            #endif

                GE_ASSERT(indices.Accessor->count % 3 == 0);

                indexCount += indices.Accessor->count;
            }

            if(indexCount)
            {
                GPU::IndexedVAO meshSettings = DEFAULT;

                SetupMeshMaterials(meshSettings, file.get(), meshIn);
                SetupMeshFields(meshSettings, vertexCount, indexCount / 3);
                ProcessSubmesh(meshSettings, file.get(), meshIn);

                meshOut.VAO = ptr_create<API::IndexedVAO>(window, move(meshSettings));
            }
            else
            {
                GPU::VAO meshSettings = DEFAULT;

                SetupMeshMaterials(meshSettings, file.get(), meshIn);
                SetupMeshFields(meshSettings, vertexCount);
                ProcessSubmesh(meshSettings, file.get(), meshIn);

                meshOut.VAO = ptr_create<API::VAO>(window, move(meshSettings));
            }
        }

        skeletons = Array<Skeleton>(file->skins.size());
        for(size_t i = 0; i < file->skins.size(); i++)
        {
            const gltf::Skin& skeletonIn = file->skins[i];
            Skeleton& skeletonOut = skeletons[i];

            skeletonOut.Name = skeletonIn.name;
            skeletonOut.Bones = Array<Bone>(skeletonIn.joints.size());

            for(size_t b = 0; b < skeletonIn.joints.size(); b++)
            {
                const gltf::Node& boneIn = file->nodes[skeletonIn.joints[b]];
                Bone& boneOut = skeletonOut.Bones[b];

                boneOut.Name = boneIn.name;
                boneOut.Transform = GetTransformFromNode(boneIn);
                boneOut.InverseBindMatrix = glm::inverse(boneOut.Transform.ToMat4());

                for(size_t childIndex : boneIn.children)
                {
                    childIndex = std::ranges::find(skeletonIn.joints, childIndex) - skeletonIn.joints.begin();
                    Bone& child = skeletonOut.Bones[childIndex];

                    child.Parent.Name = boneIn.name;
                    child.Parent.Location = b;
                    child.Parent.Pointer = &boneOut;
                }
            }
        }
    }

    void ReadGLTFAsFile(Window* window, const std::filesystem::path& path, Array<File>& files)
    {
        const Path parentDirectory = path.parent_path();

        Array<Mesh> meshes;
        Array<Skeleton> skeletons;
        ReadGLTF(window, path, meshes, skeletons);

        files = Array<File>(meshes.Count() + skeletons.Count());
        size_t fileOffset = 0;

        for(size_t i = 0; i < meshes.Count(); i++, fileOffset++)
        {
            Mesh& mesh = meshes[i];
            Path filePath = parentDirectory / Path(mesh.Name).replace_extension(Mesh::Type.Extension);

            files[fileOffset] = File(window, filePath, std::move(mesh));
        }

        for(size_t i = 0; i < skeletons.Count(); i++, fileOffset++)
        {
            Skeleton& skeleton = skeletons[i];
            Path filePath = parentDirectory / Path(skeleton.Name).replace_extension(Skeleton::Type.Extension);

            files[fileOffset] = File(window, filePath, std::move(skeleton));
        }
    }

    PrimitiveData GetAttributeData(const gltf::Asset& file, const gltf::Primitive& prim, std::string_view attribute)
    {
        PrimitiveData data = DEFAULT;

        data.Attribute = prim.findAttribute(attribute);
        if(data.Attribute == prim.attributes.end()) return data;

        data.Accessor = &file.accessors[data.Attribute->accessorIndex];

        if(data.Accessor->bufferViewIndex.has_value())
            data.BufferView = &file.bufferViews[data.Accessor->bufferViewIndex.value()];
        else
            return data;

        data.Buffer = &file.buffers[data.BufferView->bufferIndex];

        return data;
    }

    PrimitiveData GetIndicesData(const gltf::Asset& file, const gltf::Primitive& prim)
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

    void SetupMeshFields(GPU::VAO& meshOut, size_t vertexCount)
    {
        meshOut.Counts.BufferCount = 1; // INTERLEAVED
        meshOut.Counts.FieldCount = 4; // POS UV NOR TAN

        GPU::Buffer<std::byte>& buf = meshOut.Buffers[0];
        buf.Stride = sizeof(Vertex);
        buf.Data = Array<std::byte>(vertexCount * buf.Stride);
        buf.UsageHint = GPU::BufferUsageHint::Default;

        meshOut.Fields[0] = POSITION_FIELD;
        meshOut.Fields[1] = UV_FIELD;
        meshOut.Fields[2] = NORMAL_FIELD;
        meshOut.Fields[3] = TANGENT_FIELD;
    }

    void SetupMeshFields(GPU::IndexedVAO& meshOut, size_t vertexCount, size_t triCount)
    {
        SetupMeshFields(meshOut, vertexCount);

        meshOut.TriangleFormat = GL_UNSIGNED_INT;

        GPU::Buffer<std::byte>& buf = meshOut.TriangleBuffer;
        buf.Stride = sizeof(Face);
        buf.Data = Array<std::byte>(triCount * buf.Stride);
        buf.UsageHint = GPU::BufferUsageHint::Default;
    }

    void SetupMeshMaterials(GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& mesh)
    {
        GE_ASSERT(mesh.primitives.size() < GE_MAX_VAO_MATERIAL);

        meshOut.Counts.MaterialCount = mesh.primitives.size();

        size_t offset = 0;
        for(size_t i = 0; i < mesh.primitives.size(); i++)
        {
            const gltf::Primitive& prim = mesh.primitives[i];
            GPU::MaterialSlot& slot = meshOut.Materials[i];

            if(const PrimitiveData indices = GetIndicesData(file, prim))
            {
                slot.Count = indices.Accessor->count / 3;
                slot.Offset = offset;
                offset += slot.Count;
                continue;
            }

            const PrimitiveData position = GetAttributeData(file, prim, "POSITION");
            GE_ASSERT(position);

            slot.Count = position.Accessor->count / 3;
            slot.Offset = offset;
            offset += slot.Count;
        }
    }

    void ProcessSubmesh(GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn)
    {
        GPU::Buffer<std::byte>& buf = meshOut.Buffers[0];
        GE_ASSERT(buf.GetByteCount() % sizeof(Vertex) == 0);

        size_t offset = 0;
        for(const gltf::Primitive& submeshIn : meshIn.primitives)
        {
            GE_ASSERT(submeshIn.type == gltf::PrimitiveType::Triangles);

            const PrimitiveData position = GetAttributeData(file, submeshIn, "POSITION");
            const PrimitiveData uv = GetAttributeData(file, submeshIn, "TEXCOORD_0");
            const PrimitiveData normal = GetAttributeData(file, submeshIn, "NORMAL");
            const PrimitiveData tangent = GetAttributeData(file, submeshIn, "TANGENT");

            const std::span writeSpan { (Vertex*) buf.Data.begin() + offset, (Vertex*) buf.Data.end() };

            GE_MODEL_CHECK_ATTR(POSITION_FIELD, position, (ConversionFunc<glm::vec3, glm::vec3>) nullptr);
            GE_MODEL_CHECK_ATTR(UV_FIELD, uv, (ConversionFunc<glm::vec2, glm::vec2>) nullptr);
            GE_MODEL_CHECK_ATTR(NORMAL_FIELD, normal, ConvertNormal);
            GE_MODEL_CHECK_ATTR(TANGENT_FIELD, tangent, ConvertTangent);

            offset += position.Accessor->count;
        }
    }

    void ProcessSubmesh(GPU::IndexedVAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn)
    {
        ProcessSubmesh((GPU::VAO&) meshOut, file, meshIn);

        GPU::Buffer<std::byte>& buf = meshOut.TriangleBuffer;
        GE_ASSERT(buf.GetByteCount() % sizeof(Face) == 0);

        size_t offset = 0;
        for(const gltf::Primitive& submeshIn : meshIn.primitives)
        {
            const PrimitiveData indices = GetIndicesData(file, submeshIn);
            const gltf::Accessor& accessor = *indices.Accessor;
            const gltf::BufferView& bufView = *indices.BufferView;
            const std::span writeSpan { ((u32*) buf.Data.begin()) + offset, (u32*) buf.Data.end() };
            const size_t stride = bufView.byteStride.value_or(gltf::getElementByteSize(accessor.type, accessor.componentType));

            GE_ASSERT(submeshIn.type == gltf::PrimitiveType::Triangles)
            GE_ASSERT(accessor.count % 3 == 0);
            GE_ASSERT(indices.Accessor);

            const gltf::sources::Array* data = std::get_if<gltf::sources::Array>(&indices.Buffer->data);
            GE_ASSERT(data);

            for(size_t i = 0; i < accessor.count; i++)
            {
                u32* dst = &writeSpan[i];

                if(accessor.componentType == gltf::ComponentType::UnsignedShort)
                {
                    const u16* src = (const u16*) &data->bytes[bufView.byteOffset + accessor.byteOffset + stride * i];
                    *dst = *src;
                }
                else if(accessor.componentType == gltf::ComponentType::UnsignedInt)
                {
                    const u32* src = (const u32*) &data->bytes[bufView.byteOffset + accessor.byteOffset + stride * i];
                    *dst = *src;
                }
                else GE_FAIL("Unsupported index type!");
            }

            offset += accessor.count;
        }
    }

    TransformData GetTransformFromNode(const gltf::Node& node)
    {
        TransformData result;
        if(const gltf::TRS* trs = std::get_if<gltf::TRS>(&node.transform))
        {
            result.Position = std::bit_cast<glm::vec3>(trs->translation);
            result.Rotation = std::bit_cast<glm::quat>(trs->rotation);
            result.Scale = std::bit_cast<glm::vec3>(trs->scale);
        }
        else
        {
            const gltf::math::fmat4x4* mat = std::get_if<gltf::math::fmat4x4>(&node.transform);
            Decompose(*(const glm::mat4*) mat, result.Position, result.Rotation, result.Scale);
        }

        // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
        return result;
    }
}
