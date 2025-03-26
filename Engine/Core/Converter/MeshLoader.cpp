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
    void SetupMeshFields(GPU::VAO& meshOut, size_t vertexCount);
    void SetupMeshFields(GPU::IndexedVAO& meshOut, size_t vertexCount, size_t triCount);
    void SetupMeshWeights(GPU::VAO& meshOut, size_t vertexCount);
    void SetupMeshMaterials(GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& mesh);
    void ProcessSubmesh(const GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn, const GLTFImportSettings& settings);
    void ProcessSubmesh(const GPU::IndexedVAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn, const GLTFImportSettings& settings);
    void ProcessSubmeshWeights(const GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn);
    float ProcessChannel(AnimationChannel& channelOut, const gltf::Asset& file, const gltf::Animation& animationIn,
                       const gltf::AnimationChannel& channelIn, float scale);

    TransformData GetTransformFromNode(const gltf::Node& node);

#ifdef GE_ENABLE_IMGUI
    void GLTFImportSettings::OnEditorGUI(u8 depth)
    {
        DrawField(ScalarField{ "Bone Scale", "", FLT_EPSILON }, BoneScale, depth);
        DrawField(Field{ "Flip Tangent", "" }, FlipTangents, depth);
    }
#endif

    void ReadGLTF(Window* window, const Path& path, const GLTFImportSettings& settings, GLTFResult& result)
    {
        gltf::Expected<gltf::GltfDataBuffer> data = gltf::GltfDataBuffer::FromPath(path);
        if(data.error() != gltf::Error::None)
            return Log::Error(std::format("Failed to open {}.", path.string()));

        gltf::Parser parser;
        gltf::Expected<gltf::Asset> file = parser.loadGltf(data.get(), path.parent_path(), GLTF_LOAD_OPTIONS);
        if(data.error() != gltf::Error::None)
            return Log::Error(std::format("Failed to parse {}.", path.string()));

        result.Meshes = Array<Mesh>(file->meshes.size());
        for(size_t i = 0; i < file->meshes.size(); i++)
        {
            const gltf::Mesh& meshIn = file->meshes[i];
            Mesh& meshOut = result.Meshes[i];

            meshOut.Name = meshIn.name;

            size_t vertexCount = 0;
            size_t indexCount = 0;
            bool hasWeights = false;
            for(const gltf::Primitive& subMesh : meshIn.primitives)
            {
                const PrimitiveData position = GetAttributeData(file.get(), subMesh, "POSITION");
                const PrimitiveData indices = GetIndicesData(file.get(), subMesh);

                GE_ASSERT(position);
                vertexCount += position->Accessor->count;

                if(!indices->Accessor) return;

            #ifdef DEBUG
                if(indexCount) GE_ASSERT(indices->Accessor->count);
            #endif

                GE_ASSERT(indices->Accessor->count % 3 == 0);

                indexCount += indices->Accessor->count;
                hasWeights |= subMesh.findAttribute("JOINTS_0") != subMesh.attributes.end();
            }

            if(indexCount)
            {
                GPU::IndexedVAO meshSettings = DEFAULT;

                SetupMeshMaterials(meshSettings, file.get(), meshIn);
                SetupMeshFields(meshSettings, vertexCount, indexCount / 3);
                ProcessSubmesh(meshSettings, file.get(), meshIn, settings);

                if(hasWeights)
                {
                    SetupMeshWeights(meshSettings, vertexCount);
                    ProcessSubmeshWeights(meshSettings, file.get(), meshIn);
                }

                meshOut.VAO = ptr_create<API::IndexedVAO>(window, move(meshSettings));
            }
            else
            {
                GPU::VAO meshSettings = DEFAULT;

                SetupMeshMaterials(meshSettings, file.get(), meshIn);
                SetupMeshFields(meshSettings, vertexCount);
                ProcessSubmesh(meshSettings, file.get(), meshIn, settings);

                if(hasWeights)
                {
                    SetupMeshWeights(meshSettings, vertexCount);
                    ProcessSubmeshWeights(meshSettings, file.get(), meshIn);
                }

                meshOut.VAO = ptr_create<API::VAO>(window, move(meshSettings));
            }
        }

        result.Skeletons = Array<Skeleton>(file->skins.size());
        for(size_t i = 0; i < file->skins.size(); i++)
        {
            const gltf::Skin& skeletonIn = file->skins[i];
            Skeleton& skeletonOut = result.Skeletons[i];

            skeletonOut.Name = skeletonIn.name;
            skeletonOut.Bones = Array<Bone>(skeletonIn.joints.size());

            Array<glm::mat4> modelMatrices = Array<glm::mat4>(skeletonIn.joints.size());
            for(size_t b = 0; b < skeletonIn.joints.size(); b++)
            {
                const gltf::Node& boneIn = file->nodes[skeletonIn.joints[b]];
                Bone& boneOut = skeletonOut.Bones[b];

                boneOut.Name = boneIn.name;
                boneOut.Transform = GetTransformFromNode(boneIn);
                boneOut.Transform.Position *= settings.BoneScale;

                if(boneOut.Parent)
                    modelMatrices[b] = modelMatrices[skeletonOut.GetIndex(boneOut.Parent.Pointer)] * boneOut.Transform.ToMat4();
                else
                    modelMatrices[b] = boneOut.Transform.ToMat4();

                boneOut.InverseBindMatrix = glm::inverse(modelMatrices[b]);

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

        result.Animations = Array<Animation>(file->animations.size());
        for (size_t i = 0; i < file->animations.size(); i++)
        {
            const gltf::Animation& animationIn = file->animations[i];
            Animation& animationOut = result.Animations[i];

            animationOut.Name = animationIn.name;

            animationOut.Channels = Array<AnimationChannel>(animationIn.channels.size());
            for(size_t c = 0; c < animationIn.channels.size(); c++)
            {
                const gltf::AnimationChannel& channelIn = animationIn.channels[c];
                AnimationChannel& channelOut = animationOut.Channels[c];

                if(!channelIn.nodeIndex.has_value()) continue;

                animationOut.Length = std::max(animationOut.Length, ProcessChannel(channelOut, file.get(), animationIn, channelIn, settings.BoneScale));
            }
        }
    }

    void ReadGLTFAsFile(Window* window, const std::filesystem::path& path, const GLTFImportSettings& settings, Array<File>& files)
    {
        const Path parentDirectory = path.parent_path();

        GLTFResult result;
        ReadGLTF(window, path, settings, result);

        files = Array<File>(result.Meshes.Size() + result.Skeletons.Size() + result.Animations.Size());
        size_t fileOffset = 0;

        for(size_t i = 0; i < result.Meshes.Size(); i++, fileOffset++)
        {
            Mesh& mesh = result.Meshes[i];
            Path filePath = parentDirectory / Path(mesh.Name).replace_extension(Mesh::SType.Extension);

            files[fileOffset] = File(window, filePath, std::move(mesh));
        }

        for(size_t i = 0; i < result.Skeletons.Size(); i++, fileOffset++)
        {
            Skeleton& skeleton = result.Skeletons[i];
            Path filePath = parentDirectory / Path(skeleton.Name).replace_extension(Skeleton::SType.Extension);

            files[fileOffset] = File(window, filePath, std::move(skeleton));
        }

        for(size_t i = 0; i < result.Animations.Size(); i++, fileOffset++)
        {
            Animation& animation = result.Animations[i];
            Path filePath = parentDirectory / Path(animation.Name).replace_extension(Animation::SType.Extension);

            files[fileOffset] = File(window, filePath, std::move(animation));
        }
    }

    AccessorData GetAccessorData(const gltf::Asset& file, size_t index)
    {
        AccessorData result = DEFAULT;

        result.Accessor = &file.accessors[index];

        if(!result.Accessor->bufferViewIndex.has_value()) return result;

        result.View = &file.bufferViews[result.Accessor->bufferViewIndex.value()];
        result.Buffer = &file.buffers[result.View->bufferIndex];

        return result;
    }

    PrimitiveData GetAttributeData(const gltf::Asset& file, const gltf::Primitive& prim, std::string_view attribute)
    {
        PrimitiveData result = DEFAULT;

        result.Attribute = prim.findAttribute(attribute);
        if(result.Attribute == prim.attributes.end()) return result;

        result.Accessor = GetAccessorData(file, result.Attribute->accessorIndex);

        return result;
    }

    PrimitiveData GetIndicesData(const gltf::Asset& file, const gltf::Primitive& prim)
    {
        PrimitiveData result = DEFAULT;

        if(!prim.indicesAccessor.has_value())
            return result;

        result.Accessor = GetAccessorData(file, prim.indicesAccessor.value());

        return result;
    }

    ChannelData GetChannelData(const gltf::Asset& file, const gltf::Animation& animation, const gltf::AnimationChannel& channel)
    {
        ChannelData result = DEFAULT;

        result.Channel = &channel;
        result.Sampler = &animation.samplers[channel.samplerIndex];
        result.Input = GetAccessorData(file, result.Sampler->inputAccessor);
        result.Output = GetAccessorData(file, result.Sampler->outputAccessor);

        return result;
    }

    void SetupMeshFields(GPU::VAO& meshOut, size_t vertexCount)
    {
        meshOut.AddBuffer(GPU::Buffer<std::byte>(vertexCount * sizeof(Vertex), nullptr, sizeof(Vertex)));

        meshOut.AddField(POSITION_FIELD);
        meshOut.AddField(UV_FIELD);
        meshOut.AddField(NORMAL_FIELD);
        meshOut.AddField(TANGENT_FIELD);
    }

    void SetupMeshFields(GPU::IndexedVAO& meshOut, size_t vertexCount, size_t triCount)
    {
        SetupMeshFields(meshOut, vertexCount);

        meshOut.IndicesFormat = GLType<u32>;
        meshOut.IndicesBuffer = GPU::Buffer<std::byte>(triCount * sizeof(Face), nullptr, sizeof(Face));
    }

    void SetupMeshWeights(GPU::VAO& meshOut, size_t vertexCount)
    {
        meshOut.AddBuffer(GPU::Buffer<std::byte>(vertexCount * sizeof(VertexWeight), nullptr, sizeof(VertexWeight)));
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
                slot.Count = indices->Accessor->count / 3;
                slot.Offset = offset;
                offset += slot.Count;
                continue;
            }

            const PrimitiveData position = GetAttributeData(file, prim, "POSITION");
            GE_ASSERT(position);

            slot.Count = position->Accessor->count / 3;
            slot.Offset = offset;
            offset += slot.Count;
        }
    }

    void ProcessSubmesh(const GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn, const GLTFImportSettings& settings)
    {
        const GPU::Buffer<std::byte>& buf = meshOut.Buffers[0];
        GE_ASSERT(buf.GetByteSize() % sizeof(Vertex) == 0);

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
            GE_MODEL_CHECK_ATTR(TANGENT_FIELD, tangent, settings.FlipTangents ? ConvertTangentFlipped : ConvertTangent);

            offset += position->Accessor->count;
        }
    }

    void ProcessSubmesh(const GPU::IndexedVAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn, const GLTFImportSettings& settings)
    {
        ProcessSubmesh((GPU::VAO&) meshOut, file, meshIn, settings);

        const GPU::Buffer<std::byte>& buf = meshOut.IndicesBuffer;
        GE_ASSERT(buf.GetByteSize() % sizeof(Face) == 0);

        size_t indexOffset = 0;
        size_t vertexOffset = 0;
        for(const gltf::Primitive& submeshIn : meshIn.primitives)
        {
            const PrimitiveData indices = GetIndicesData(file, submeshIn);
            const PrimitiveData positions = GetAttributeData(file, submeshIn, "POSITION");
            const gltf::Accessor& accessor = *indices->Accessor;
            const gltf::BufferView& bufView = *indices->View;
            const std::span writeSpan { ((u32*) buf.Data.begin()) + indexOffset, (u32*) buf.Data.end() };
            const size_t stride = bufView.byteStride.value_or(gltf::getElementByteSize(accessor.type, accessor.componentType));

            GE_ASSERT(submeshIn.type == gltf::PrimitiveType::Triangles);
            GE_ASSERT(accessor.count % 3 == 0);
            GE_ASSERT(indices->Accessor);

            const gltf::sources::Array* data = std::get_if<gltf::sources::Array>(&indices->Buffer->data);
            GE_ASSERT(data);

            for(size_t i = 0; i < accessor.count; i++)
            {
                u32* dst = &writeSpan[i];

                if(accessor.componentType == gltf::ComponentType::UnsignedShort)
                    *dst = AccessBuffer<u16>(indices.Accessor, i, stride) + vertexOffset;
                else if(accessor.componentType == gltf::ComponentType::UnsignedInt)
                    *dst = AccessBuffer<u32>(indices.Accessor, i, stride) + vertexOffset;
                else GE_FAIL("Unsupported index type!");
            }

            indexOffset += accessor.count;
            vertexOffset += positions->Accessor->count;
        }
    }

    void ProcessSubmeshWeights(const GPU::VAO& meshOut, const gltf::Asset& file, const gltf::Mesh& meshIn)
    {
        const GPU::Buffer<std::byte>& buf = meshOut.Buffers[1];
        GE_ASSERT(buf.GetByteSize() % sizeof(VertexWeight) == 0);

        size_t offset = 0;
        for(const gltf::Primitive& submeshIn : meshIn.primitives)
        {
            GE_ASSERT(submeshIn.type == gltf::PrimitiveType::Triangles);

            const PrimitiveData bones = GetAttributeData(file, submeshIn, "JOINTS_0");
            const PrimitiveData weights = GetAttributeData(file, submeshIn, "WEIGHTS_0");

            const std::span writeSpan { (VertexWeight*) buf.Data.begin() + offset, (VertexWeight*) buf.Data.end() };

            GE_MODEL_CHECK_ATTR(BONES_FIELD, bones, (ConversionFunc<glm::u8vec4, glm::u8vec4>) nullptr);
            GE_MODEL_CHECK_ATTR(WEIGHTS_FIELD, weights, ConvertWeight);

            offset += bones->Accessor->count;
        }
    }

    float ProcessChannel(AnimationChannel& channelOut, const gltf::Asset& file, const gltf::Animation& animationIn,
                        const gltf::AnimationChannel& channelIn, float scale)
    {
        const ChannelData channelData = GetChannelData(file, animationIn, channelIn);

        channelOut.Target.Name = file.nodes[channelIn.nodeIndex.value_or(0)].name;
        channelOut.Frames = Array<Frame>(channelData.Input.Accessor->count);
        channelOut.Type = (ChannelType) channelIn.path;

        for(size_t i = 0; i < channelOut.Frames.Size(); i++)
        {
            Frame& frameOut = channelOut.Frames[i];

            frameOut.Time = AccessBuffer<float>(channelData.Input, i);

            if(channelOut.Type == ChannelType::Location)
                frameOut.Value = AccessBuffer<glm::vec3>(channelData.Output, i) * scale;
            else if(channelOut.Type == ChannelType::Scale)
                frameOut.Value = AccessBuffer<glm::vec3>(channelData.Output, i);
            else
                frameOut.Value = AccessBuffer<glm::quat>(channelData.Output, i);

#ifdef GE_ENABLE_IMGUI
            frameOut.Channel = &channelOut;
#endif
        }

        return std::get<std::vector<double, std::pmr::polymorphic_allocator<double>>>(channelData.Input.Accessor->max)[0];
    }

    TransformData GetTransformFromNode(const gltf::Node& node)
    {
        TransformData result;
        if(const gltf::TRS* trs = std::get_if<gltf::TRS>(&node.transform))
        {
            result.Location = std::bit_cast<glm::vec3>(trs->translation);
            result.Rotation = std::bit_cast<glm::quat>(trs->rotation);
            result.Scale = std::bit_cast<glm::vec3>(trs->scale);
        }
        else
        {
            const gltf::math::fmat4x4* mat = std::get_if<gltf::math::fmat4x4>(&node.transform);
            Decompose(*(const glm::mat4*) mat, result.Location, result.Rotation, result.Scale);
        }

        return result;
    }
}
