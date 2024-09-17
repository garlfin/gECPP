//
// Created by scion on 8/10/2023.
//

#include <filesystem>
#include <fstream>
#include "Vertex.h"
#include "ASSIMP/Importer.hpp"
#include "ASSIMP/postprocess.h"
#include "ASSIMP/scene.h"

#include "Graphics/Buffer/VAO.h"

using pp = aiPostProcessSteps;

CONSTEXPR_GLOBAL unsigned POST_PROCESS =
	aiProcess_Triangulate | pp::aiProcess_FindInvalidData | pp::aiProcess_OptimizeMeshes |
	aiProcess_ImproveCacheLocality | aiProcess_FindInstances |
	aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices;

template<class T, class F>
using ConversionFunc = void(u32, const F&, T&);

template<class T, class S>
GPU::VertexField CreateField(T S::* DST, const char[4], u8 i, u8 buf);

template<class T, class S, class F>
void FillBuffer(T S::* DST, F* aiMesh::* SRC, u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf,
	const std::vector<aiMesh*>& src, ConversionFunc<T, F>* FUNC = nullptr);

template<class T>
void AllocateBuffer(u32 aiMesh::* COUNT, GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src);

void FillUVBuffer(GPU::Buffer<u8>& buf, const std::vector<aiMesh*>& src);

void TransformMesh(const std::vector<aiMesh*>&, GPU::IndexedVAO&);
void ConvertVec(u32, const aiVector3t<float>&, glm::i8vec3&);
void ConvertFace(u32, const aiFace&, glm::uvec3&);

int main(int argc, char** argv)
{
	GE_ASSERT(argc >= 3, "USAGE: in.file outDir");

	Assimp::Importer importer;
	const aiScene& scene = *importer.ReadFile(argv[1], POST_PROCESS);

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

	std::string basePath(argv[2]);

	for(const auto& src : meshes)
	{
		std::string path = basePath + src[0]->mName.data + ".gEMesh";

		GPU::IndexedVAO mesh;
		TransformMesh(src, mesh);

		WriteSerializableToFile(path.c_str(), mesh);
	}

	return 0;
}

void TransformMesh(const std::vector<aiMesh*>& src, GPU::IndexedVAO& dst)
{
	u64 vertexCount = 0;

	dst.Counts.MaterialCount = src.size();
	for(unsigned i = 0; i < dst.Counts.MaterialCount; i++)
	{
		const aiMesh& mesh = *src[i];

		dst.Materials[i] = GPU::MaterialSlot(MOVE(std::string(mesh.mName.C_Str())), mesh.mNumFaces, vertexCount);

		vertexCount += mesh.mNumFaces;
	}

	dst.Counts.BufferCount = 2;
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

		GE_ASSERT(source, "MISSING FIELD");

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
}