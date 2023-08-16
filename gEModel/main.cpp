//
// Created by scion on 8/10/2023.
//

#include "gETF.h"
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>

#define DEFINE_GL_TYPE(TYPE, VALUE) template<> constexpr u32 GLType<TYPE> = VALUE
#define IMPORT_FLAGS (aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_LimitBoneWeights | aiProcess_ImproveCacheLocality)

template<typename T, typename I>
using TransformFunc = void(*)(const T&, I&);

template<class T, class I>
constexpr bool IsVec3 = std::is_same_v<aiVector3t<T>, I>;

template<class T>
constexpr u32 GLType = 0;

DEFINE_GL_TYPE(float, GL_FLOAT);
DEFINE_GL_TYPE(u32, GL_UNSIGNED_INT);

void TransformFace(const aiFace& s, aiVector3t<u32>& v)
{
	v.x = s.mIndices[0];
	v.y = s.mIndices[1];
	v.z = s.mIndices[2];
}

void TransformUV(const aiVector3D& s, aiVector2D& v)
{
	v.x = s.x;
	v.y = s.y;
}

template<unsigned int aiMesh::* COUNT_OFFSET, typename INDIVIDUAL_TYPE, typename T, T* aiMesh::* DATA_OFFSET, typename I = T, TransformFunc<T, I> F = nullptr>
void CreateField(gETF::VertexField& field, aiMesh** source, u8 count, const char* name)
{
	field.Index = 0;
	field.Name = strcpy(new char[strlen(name) + 1], name);
	field.Type = GLType<INDIVIDUAL_TYPE>;

	field.TypeCount = IsVec3<INDIVIDUAL_TYPE, I> || std::is_same_v<aiFace, I> ? 3 : 2;

	field.Count = 0;
	for(u8 i = 0; i < count; i++) field.Count += source[i]->*COUNT_OFFSET;

	I* data = new I[field.Count];
	field.Data = data;

	for(u8 i = 0; i < count; i++)
	{
		u32 meshCount = source[i]->*COUNT_OFFSET;
		T* arr = source[i]->*DATA_OFFSET;

		if constexpr(F) for(u32 m = 0; m < meshCount; m++) F(arr[m], data[m]);
		else memcpy(data, arr, sizeof(I) * meshCount);

		data += meshCount;
	}
}

int main()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("cube.dae", IMPORT_FLAGS);

	auto* submeshCount = new u8[scene->mNumMeshes] {};
	u8 realMeshCount = 0;
	const aiMesh* previousMesh = nullptr;

	for(u8 i = 0; i < (u8) MIN(scene->mNumMeshes, UINT8_MAX); i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		if(!previousMesh || !strcmp(previousMesh->mName.data, mesh->mName.data))
		{
			submeshCount[realMeshCount]++;
			realMeshCount++;
		}

		previousMesh = mesh;
	}

	gETF::Header file {};

	file.MeshCount = realMeshCount;
	file.Meshes = new gETF::Mesh[realMeshCount];

	for(u8 mI = 0, i = 0; i < realMeshCount; mI += submeshCount[i], i++)
	{
		gETF::Mesh& mesh = file.Meshes[i];
		aiMesh** sourceMesh = &scene->mMeshes[mI];
		u32 subCount = submeshCount[i];

		mesh.TriangleMode = TRIANGLE_MODE_SIMPLE;

		mesh.FieldCount = 5; // POS UV NOR TAN TRI
		mesh.Fields = new gETF::VertexField[5];

		// I may have overcomplicated things
		CreateField<&aiMesh::mNumVertices, float, aiVector3D, &aiMesh::mVertices>(mesh.Fields[0], sourceMesh, subCount, "POS");
		CreateField<&aiMesh::mNumVertices, float, aiVector3D, &aiMesh::FirstMap, aiVector2D, TransformUV>(mesh.Fields[1], sourceMesh, subCount, "UV");
		CreateField<&aiMesh::mNumVertices, float, aiVector3D, &aiMesh::mNormals>(mesh.Fields[2], sourceMesh, subCount, "NOR");
		CreateField<&aiMesh::mNumVertices, float, aiVector3D, &aiMesh::mTangents>(mesh.Fields[3], sourceMesh, subCount, "TAN");
		CreateField<&aiMesh::mNumFaces, u32, aiFace, &aiMesh::mFaces, aiVector3t<u32>, TransformFace>(mesh.Fields[4], sourceMesh, subCount, "TRI");

		mesh.MaterialCount = submeshCount[i];
		mesh.Materials = new gETF::MaterialSlot[submeshCount[i]];
		u32 triOffset = 0;

		for(u8 m = 0; m < submeshCount[i]; m++)
		{
			gETF::MaterialSlot& mat = mesh.Materials[i];
			mat.MaterialIndex = 0;
			mat.Offset = triOffset;
			triOffset += mat.Count = sourceMesh[m]->mNumFaces;
		}
	}

	gETF::SerializationBuffer buf{};
	file.Deserialize(buf);

	FILE* output = fopen("cube.gETF", "wb");
	fwrite(buf.Data(), buf.Length(), 1, output);
	fclose(output);

	return 0;
}
