//
// Created by scion on 8/10/2023.
//

#include "gETF.h"
#include "Vertex.h"

#include "GL/Type.h"
#include "ASSIMP/Importer.hpp"
#include "ASSIMP/scene.h"
#include "ASSIMP/postprocess.h"

using pp = aiPostProcessSteps;

CONSTEXPR_GLOBAL unsigned POST_PROCESS =
	pp::aiProcess_Triangulate | pp::aiProcess_FindInvalidData | pp::aiProcess_OptimizeMeshes |
	pp::aiProcess_ImproveCacheLocality | pp::aiProcess_CalcTangentSpace | pp::aiProcess_FindInstances |
	pp::aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices;

template<class T, class F>
using ConversionFunc = void(u32, const F&, T&);

template<class T, class S>
gETF::VertexField CreateField(T S::* DST, const char[4], u8 i, u8 buf);

template<class T, class S, class F>
void FillBuffer(T S::* DST, F* aiMesh::* SRC, u32 aiMesh::* COUNT, gETF::VertexBuffer& buf,
	const std::vector<aiMesh*>& src, ConversionFunc<T, F>* FUNC = nullptr);

template<class T>
void AllocateBuffer(u32 aiMesh::* COUNT, gETF::VertexBuffer& buf, const std::vector<aiMesh*>& src);

void FillUVBuffer(gETF::VertexBuffer& buf, const std::vector<aiMesh*>& src);

void TransformMesh(const std::vector<aiMesh*>&, gETF::Mesh&);
void ConvertVec(u32, const aiVector3t<float>&, glm::i8vec3&);
void ConvertFace(u32, const aiFace&, glm::uvec3&);

int main(int argc, char** argv)
{
	GE_ASSERT(argc >= 3, "USAGE: in.file out.file!");

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

	gETF::File file;
	file.MeshCount = meshes.size();
	file.Meshes = new gE::Reference<gETF::Mesh>[meshes.size()];

	for(unsigned i = 0; i < file.MeshCount; i++)
		TransformMesh(meshes[i], file.Meshes[i] = gE::ref_create<gETF::Mesh>());

	gETF::SerializationBuffer writeBuffer;
	file.Deserialize(writeBuffer);
	writeBuffer.ToFile(argv[2]);

	return 0;
}

void TransformMesh(const std::vector<aiMesh*>& src, gETF::Mesh& dst)
{
	u64 vertexCount = 0;

	dst.MaterialCount = src.size();
	dst.Materials = new gETF::MaterialSlot[dst.MaterialCount];
	for(unsigned i = 0; i < dst.MaterialCount; i++)
	{
		const aiMesh& mesh = *src[i];
		gETF::MaterialSlot& material = dst.Materials[i];

		material.MaterialIndex = i;
		material.Count = mesh.mNumFaces;
		material.Offset = vertexCount;

		vertexCount += mesh.mNumFaces;
	}

	dst.BufferCount = 2;
	dst.FieldCount = 4;

	dst.Buffers = new gETF::VertexBuffer[dst.BufferCount];
	dst.Fields = new gETF::VertexField[dst.FieldCount];

	AllocateBuffer<Vertex>(&aiMesh::mNumVertices, dst.Buffers[0], src);
	AllocateBuffer<Face>(&aiMesh::mNumFaces, dst.Buffers[1], src);

	dst.Fields[0] = CreateField(&Vertex::Position, "POS\0", 0, 0);
	FillBuffer(&Vertex::Position, &aiMesh::mVertices, &aiMesh::mNumVertices, dst.Buffers[0], src);

	dst.Fields[1] = CreateField(&Vertex::UV, "UV0\0", 1, 0);
	FillUVBuffer(dst.Buffers[0], src);

	dst.Fields[2] = CreateField(&Vertex::Normal, "NOR\0", 2, 0);
	FillBuffer(&Vertex::Normal, &aiMesh::mNormals, &aiMesh::mNumVertices, dst.Buffers[0], src, ConvertVec);

	dst.Fields[3] = CreateField(&Vertex::Tangent, "TAN\0", 3, 0);
	FillBuffer(&Vertex::Tangent, &aiMesh::mTangents, &aiMesh::mNumVertices, dst.Buffers[0], src, ConvertVec);

	dst.TriangleMode = gETF::TriangleMode::Simple;
	dst.Triangles = CreateField(&Face::Triangle, "TRI\0", 0, 1);
	FillBuffer(&Face::Triangle, &aiMesh::mFaces, &aiMesh::mNumFaces, dst.Buffers[1], src, ConvertFace);
}

template<class T, class S>
gETF::VertexField CreateField(T S::* DST, const char name[4], u8 index, u8 bufIndex)
{
	gETF::VertexField field;

	memcpy((void*) field.Name, (void*) name, sizeof(field.Name));
	field.ElementType = GLType<typename T::value_type>;
	field.ElementCount = T::length();
	field.Index = index;

	field.BufferIndex = bufIndex;
	field.Offset = (u8) (size_t) &((S*) nullptr->*DST); // Work of the Devil?
	field.Normalized = !std::is_same_v<typename T::value_type, float>;

	return field;
}

template<class T, class S, class F>
void FillBuffer(T S::* DST, F* aiMesh::* SRC, u32 aiMesh::* COUNT, gETF::VertexBuffer& buf,
	const std::vector<aiMesh*>& src, ConversionFunc<T, F>* FUNC)
{
	S* dst = (S*) buf.Data;
	u32 offset = 0;

	for(aiMesh* mesh : src)
	{
		u32 count = mesh->*COUNT;
		F* source = mesh->*SRC;

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
	vec = glm::clamp(vec, glm::vec3(-1), glm::vec3(1.0));
	dst = vec * glm::vec3(INT8_MAX);
}

void ConvertFace(u32 offset, const aiFace& src, glm::uvec3& dst)
{
	dst = *(glm::uvec3*)src.mIndices + offset;
	std::cout << dst.x << ", " << dst.y << ", " << dst.z << '\n';
}

void FillUVBuffer(gETF::VertexBuffer& buf, const std::vector<aiMesh*>& src)
{
	auto* dst = (Vertex*) buf.Data;
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
void AllocateBuffer(u32 aiMesh::* COUNT, gETF::VertexBuffer& buf, const std::vector<aiMesh*>& src)
{
	u32 finalCount = 0;
	for(aiMesh* mesh : src) finalCount += mesh->*COUNT;

	buf.Stride = sizeof(T);
	buf.Count = finalCount;
	buf.Data = malloc(buf.Stride * buf.Count);
}