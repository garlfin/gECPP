//
// Created by scion on 8/10/2023.
//

#include "gETF.h"
#include "ASSIMP/Importer.hpp"
#include "ASSIMP/scene.h"
#include "ASSIMP/postprocess.h"

using pp = aiPostProcessSteps;

CONSTEXPR_GLOBAL unsigned POST_PROCESS =
	pp::aiProcess_Triangulate | pp::aiProcess_FindInvalidData | pp::aiProcess_OptimizeMeshes |
	pp::aiProcess_ImproveCacheLocality | pp::aiProcess_CalcTangentSpace | pp::aiProcess_FindInstances |
	pp::aiProcess_OptimizeGraph;

template<class T>
CONSTEXPR_GLOBAL GLenum GLType = 0;

template<>
CONSTEXPR_GLOBAL GLenum GLType<float> = GL_FLOAT;

template<>
CONSTEXPR_GLOBAL GLenum GLType<u32> = GL_UNSIGNED_INT;

template<class T, u8 L>
void AllocateField(char[4], u8, u32, gETF::VertexField&, gETF::VertexBuffer&);

void TransformMesh(const std::vector<aiMesh*>&, gETF::Mesh&);

int main(int argc, char** argv)
{
	GE_ASSERT(argc >= 2, "ARGS SHOULD BE AT LEAST ONE!");

	Assimp::Importer importer;
	const aiScene& scene = *importer.ReadFile(argv[0], POST_PROCESS);

	aiMesh* previousMesh = nullptr;
	std::vector<std::vector<aiMesh*>> meshes;
	for(unsigned i = 0; i < scene.mNumMeshes; i++)
	{
		aiMesh* mesh = scene.mMeshes[i];

		if(!previousMesh || mesh->mName != previousMesh->mName)
			meshes.emplace_back(1).push_back(mesh);
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
	writeBuffer.ToFile(argv[1]);
}

void TransformMesh(const std::vector<aiMesh*>& src, gETF::Mesh& dst)
{
	u64 vertexCount = 0, triangleCount = 0;

	dst.MaterialCount = src.size();
	for(unsigned i = 0; i < dst.MaterialCount; i++)
	{
		const aiMesh& mesh = *src[i];
		gETF::MaterialSlot& material = dst.Materials[i];

		vertexCount += mesh.mNumVertices;
		triangleCount += mesh.mNumFaces;

		material.MaterialIndex = i;
		material.Count = mesh.mNumFaces;
		material.Offset = triangleCount;
	}

	dst.BufferCount = 5;
	dst.FieldCount = 4;

	dst.Buffers = new gETF::VertexBuffer[dst.BufferCount];
	dst.Fields = new gETF::VertexField[dst.BufferCount];

	AllocateField<float, 3>("POS\0", 0, vertexCount, dst.Fields[0], dst.Buffers[0]);
	AllocateField<float, 2>("UV0\0", 1, vertexCount, dst.Fields[1], dst.Buffers[1]);
	AllocateField<float, 3>("NOR\0", 2, vertexCount, dst.Fields[2], dst.Buffers[2]);
	AllocateField<float, 3>("TAN\0", 3, vertexCount, dst.Fields[3], dst.Buffers[3]);

	dst.TriangleMode = gETF::TriangleMode::Simple;
	AllocateField<u32, 3>("TRI\0", 4, triangleCount, dst.Triangles, dst.Buffers[4]);
}

template<class T, u8 L>
void AllocateField(char name[], u8 index, u32 count, gETF::VertexField& field, gETF::VertexBuffer& buf)
{
	memcpy((void*) field.Name, (void*) name, sizeof(field.Name));
	field.ElementType = GLType<T>;
	field.ElementCount = L;
	field.Index = index;
	field.BufferIndex = index;
	field.Offset = 0;

	buf.Count = count;
	buf.Stride = sizeof(T) * L;
	buf.Data = malloc(buf.Count * buf.Stride);
}