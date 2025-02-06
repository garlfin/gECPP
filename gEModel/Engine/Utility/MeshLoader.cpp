//
// Created by scion on 11/14/2024.
//


#include "MeshLoader.h"
#include "Vertex.h"

#include <filesystem>
#include <fstream>
#include <Asset/Mesh/Mesh.h>
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/postprocess.h>
#include <ASSIMP/scene.h>
#include <Utility/AssetManager.h>
#include <Graphics/Buffer/VAO.h>

#include "Asset/Mesh/Skeleton.h"

using pp = aiPostProcessSteps;

CONSTEXPR_GLOBAL unsigned POST_PROCESS =
	aiProcess_Triangulate | aiProcess_FindInvalidData | aiProcess_OptimizeMeshes |
	aiProcess_ImproveCacheLocality | aiProcess_FindInstances |
	aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData;

template<class T, class F>
using ConversionFunc = void(u32 id, const F& from, T& to);

template<class T, class S>
GPU::VertexField CreateField(T S::* DST, const char[4], u8 index, u8 bufIndex);

template<class T, class S, class F>
void FillBuffer(T S::* DST, F* aiMesh::* SRC, u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf,
	const std::vector<aiMesh*>& src, ConversionFunc<T, F>* FUNC = nullptr);

template<class T>
void AllocateBuffer(u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src);

void FillUVBuffer(GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src);

void ConvertMesh(const std::vector<aiMesh*>&, GPU::IndexedVAO&);
void ConvertBones(const std::vector<aiMesh*>&, const gE::Skeleton&, GPU::Buffer<gE::VertexWeight>&);
void ConvertVec(u32, const aiVector3t<float>&, glm::i8vec3&);
void ConvertFace(u32, const aiFace&, glm::uvec3&);
void ConvertAABB(const aiAABB&, gE::AABB<Dimension::D3D>&);
void InsertAvailableWeight(u8 boneID, float weight, gE::VertexWeight& dst);

template<class ARG_T, void(*FUNC)(const aiNode&, ARG_T&)>
void IterateSkeleton(const aiNode& node, ARG_T& arg)
{
	FUNC(node, arg);
	for(unsigned i = 0; i < node.mNumChildren; i++)
		IterateSkeleton<ARG_T, FUNC>(*node.mChildren[i], arg);
}

inline void IterateBoneCount(const aiNode& node, size_t& size) { if(node.mParent && !node.mNumMeshes) size++;}
inline void ConvertBone(const aiNode& node, gE::Bone*& bone);

namespace gE::gEModel
{
	void ConvertFile(Window* window, const Path& source, const std::filesystem::path& output)
	{
		Path out = output.empty() ? source.parent_path() : output;

		Assimp::Importer importer = DEFAULT;
		const aiScene& scene = *importer.ReadFile(source.string(), POST_PROCESS);

		aiMesh* previousMesh = nullptr;
		std::vector<std::vector<aiMesh*>> meshes;
		for(unsigned i = 0; i < scene.mNumMeshes; i++)
		{
			aiMesh* mesh = scene.mMeshes[i];

			if(!previousMesh || mesh->mName != previousMesh->mName)
				meshes.emplace_back().push_back(mesh);
			else
				meshes.back().push_back(mesh);

			previousMesh = mesh;
		}

		Skeleton skeleton;

		size_t boneCount = 0;
		IterateSkeleton<size_t, IterateBoneCount>(*scene.mRootNode, boneCount);

		if(boneCount)
		{
			skeleton.Bones = Array<Bone>(boneCount);

			Bone* currentBone = skeleton.Bones.Data();
			IterateSkeleton<Bone*, ConvertBone>(*scene.mRootNode, currentBone);

			Path path = out / source.filename().replace_extension(".skel");
			WriteSerializableToFile(path, skeleton);

			std::cout << "Wrote skeleton to " << path << std::endl;
		}

		for(const auto& src : meshes)
		{
			GPU::IndexedVAO meshSettings;
			ConvertMesh(src, meshSettings);

			Mesh mesh;
			mesh.Name = std::string(src[0]->mName.data);
			mesh.Bounds = AABB<Dimension::D3D>(glm::vec3(FP_PINF), glm::vec3(FP_NINF));

			bool hasSkeleton = false;

			for(aiMesh* subMesh : src)
			{
				AABB<Dimension::D3D> aabb;
				ConvertAABB(subMesh->mAABB, aabb);
				mesh.Bounds = mesh.Bounds & aabb;

				hasSkeleton |= subMesh->HasBones();
			}

			mesh.VAO = gE::ptr_cast<API::IVAO>(new API::IndexedVAO(window, move(meshSettings)));

			/*if(hasSkeleton)
			{
				GPU::Buffer<VertexWeight> weightSettings;
				ConvertBones(src, skeleton, weightSettings);

				mesh.BoneWeights = ptr_create<API::Buffer<VertexWeight>>(window, move(weightSettings));
			}*/

			Path path = out / (mesh.Name + ".mesh");
			WriteSerializableToFile(path, mesh);

			std::cout << "Wrote to " << path << std::endl;
		}
	}
}

void ConvertMesh(const std::vector<aiMesh*>& src, GPU::IndexedVAO& dst)
{
	u64 vertexCount = 0;

	dst.Counts.MaterialCount = src.size();
	for(unsigned i = 0; i < dst.Counts.MaterialCount; i++)
	{
		const aiMesh& mesh = *src[i];

		dst.Materials[i] = GPU::MaterialSlot(move(std::string(mesh.mName.C_Str())), vertexCount, mesh.mNumFaces);

		vertexCount += mesh.mNumFaces;
	}

	dst.Counts.BufferCount = 1;
	dst.Counts.FieldCount = 4;

	AllocateBuffer<Vertex>(&aiMesh::mNumVertices, dst.Buffers[0], src);

	dst.Fields[0] = CreateField(&Vertex::Position, "POS\0", 0, 0);
	FillBuffer(&Vertex::Position, &aiMesh::mVertices, &aiMesh::mNumVertices, dst.Buffers[0], src);

	dst.Fields[1] = CreateField(&Vertex::UV, "UV0\0", 1, 0);
	FillUVBuffer(dst.Buffers[0], src);

	dst.Fields[2] = CreateField(&Vertex::Normal, "NOR\0", 2, 0);
	FillBuffer(&Vertex::Normal, &aiMesh::mNormals, &aiMesh::mNumVertices, dst.Buffers[0], src, ConvertVec);

	dst.Fields[3] = CreateField(&Vertex::Tangent, "TAN\0", 3, 0);
	FillBuffer(&Vertex::Tangent, &aiMesh::mTangents, &aiMesh::mNumVertices, dst.Buffers[0], src, ConvertVec);

	dst.TriangleFormat = GLType<u32>;
	AllocateBuffer<Face>(&aiMesh::mNumFaces, dst.TriangleBuffer, src);
	FillBuffer(&Face::Triangle, &aiMesh::mFaces, &aiMesh::mNumFaces, dst.TriangleBuffer, src, ConvertFace);
}

template<class T, class S>
GPU::VertexField CreateField(T S::* DST, const char name[4], u8 index, u8 bufIndex)
{
	GPU::VertexField field;

	memcpy(field.Name, name, sizeof(field.Name));
	field.ElementType = GLType<typename T::value_type>;
	field.ElementCount = T::length();
	field.Index = index;

	field.BufferIndex = bufIndex;
	field.Offset = (u8) (size_t) &((S*) nullptr->*DST); // Work of the Devil?
	field.Normalized = !std::is_same_v<typename T::value_type, float>;

	return field;
}

template<class T, class S, class F>
void FillBuffer(T S::* DST, F* aiMesh::* SRC, u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf,
	const std::vector<aiMesh*>& src, ConversionFunc<T, F>* FUNC)
{
	S* dst = (S*) buf.Data.Data();
	u32 offset = 0;

	for(aiMesh* mesh : src)
	{
		u32 count = mesh->*COUNT;
		F* source = mesh->*SRC;

		GE_ASSERTM(source, "MISSING FIELD");

		for(u32 i = 0; i < count; i++)
		{
			if(FUNC) FUNC(offset, source[i], dst->*DST);
			else dst->*DST = *(T*) &source[i];
			dst++;
		}

		offset += mesh->mNumVertices;
	}
}

void ConvertVec(u32, const aiVector3t<float>& src, glm::i8vec3& dst)
{
	glm::vec3 vec = *(glm::vec3*) &src; // Work of an Angel?
	vec = clamp(vec, glm::vec3(-1), glm::vec3(1.0));
	dst = vec * glm::vec3(INT8_MAX);
}

void ConvertFace(u32 offset, const aiFace& src, glm::uvec3& dst)
{
	dst = *(glm::uvec3*)src.mIndices + offset;
}

void FillUVBuffer(GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src)
{
	auto* dst = (Vertex*) buf.Data.Data();
	u32 offset = 0;

	for(aiMesh* mesh : src)
	{
		u32 count = mesh->mNumVertices;

		for(u32 i = 0; i < count; i++)
		{
			dst->UV = *(glm::vec2*) &mesh->mTextureCoords[0][i];
			dst++;
		}

		offset += mesh->mNumVertices;
	}
}

template<class T>
void AllocateBuffer(u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src)
{
	u32 finalCount = 0;
	for(aiMesh* mesh : src) finalCount += mesh->*COUNT;

	buf.Stride = sizeof(T);
	buf.Data = Array<u8>(buf.Stride * finalCount);
};

void ConvertBones(const std::vector<aiMesh*>& src, const gE::Skeleton& skeleton, GPU::Buffer<gE::VertexWeight>& dst)
{
	u64 vertexCount = 0;
	for(const aiMesh* mesh : src)
		vertexCount += mesh->mNumVertices;

	dst = GPU::Buffer<gE::VertexWeight>(vertexCount);

	for(const aiMesh* mesh : src)
		for(u32 b = 0; b < mesh->mNumBones; b++)
		{
			const aiBone& bone = *mesh->mBones[b];
			const std::string boneName = std::string(bone.mName.C_Str());
			const gE::Bone* foundBone = skeleton.FindBone(boneName);

			GE_ASSERTM(foundBone, "COULD NOT FIND BONE!");
			const u8 boneIndex = skeleton.GetIndex(*foundBone);

			for(u32 w = 0; w < bone.mNumWeights; w++)
			{
				const aiVertexWeight& weight = bone.mWeights[w];
				InsertAvailableWeight(boneIndex, weight.mWeight, dst.Data[weight.mVertexId]);
			}
		}
}

void ConvertAABB(const aiAABB& src, gE::AABB<Dimension::D3D>& dst)
{
	dst.Min = *(glm::vec3*) &src.mMin;
	dst.Max = *(glm::vec3*) &src.mMax;
}

void InsertAvailableWeight(u8 boneID, float weight, gE::VertexWeight& dst)
{
	for(int i = 0; i < 4; i++)
		if(!dst.IDs[i])
		{
			dst.Weights[i] = (u8) (weight * UINT8_MAX);
			dst.IDs[i] = boneID;
			return;
		}

	GE_ASSERTM(false, "NUMBER OF WEIGHTS EXCEEDED 4!");
}

void ConvertBone(const aiNode& node, gE::Bone*& bone)
{
	if(!node.mParent || node.mNumMeshes) return;

	bone->Name = std::string(node.mName.C_Str());

	aiVector3D scale, position;
	aiQuaternion rotation;
	node.mTransformation.Decompose(scale, rotation, position);

	bone->Transform.Location = *(glm::vec3*) &position;
	bone->Transform.Scale = *(glm::vec3*) &scale;
	bone->Transform.Rotation = *(glm::quat*) &rotation;

	bone++;
}
