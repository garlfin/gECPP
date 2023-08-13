//
// Created by scion on 8/10/2023.
//

#include "gETF.h"
#include <ASSIMP/Importer.hpp>
#include <ASSIMP/scene.h>
#include <ASSIMP/postprocess.h>

#define MAX(x, y) ((x) < (y) ? (x) : (y))
#define IMPORT_FLAGS aiProcessPreset_TargetRealtime_Fast
#define FORMAT_VEC3 0
#define FORMAT_TEXCOORD 1
#define FORMAT_TRIANGLES_BASIC 2

template<u8 T>
void CreateField(u8 count, aiMesh** mesh, size_t offset, gETF::VertexField& field, const char* name)
{
	field.Count = 0;
	for(u8 i = 0; i < count; i++) field.Count += mesh[i]->mNumVertices;

	field.Data = new aiVector3D[field.Count];
	auto* vecPtr = (aiVector3D*) field.Data;

	for(u8 i = 0; i < count; i++)
	{
		aiMesh& submesh = *mesh[i];

		memcpy(vecPtr, *(u8**)((u8*) &submesh + offset), submesh.mNumVertices * sizeof(aiVector3D));

		vecPtr += submesh.mNumVertices;
	}

	field.Type = GL_FLOAT;
	field.Index = 0;
	field.Name = name;
	field.TypeCount = 3;
}

template<>
void CreateField<FORMAT_TEXCOORD>(u8 count, aiMesh** mesh, size_t offset, gETF::VertexField& field, const char* name)
{
	field.Count = 0;
	for(u8 i = 0; i < count; i++) field.Count += mesh[i]->mNumVertices;

	field.Data = new aiVector2D[field.Count];
	auto* vecPtr = (aiVector2D*) field.Data;

	for(u8 i = 0; i < count; i++)
	{
		aiMesh& submesh = *mesh[i];
		aiVector3D* texcoord = submesh.mTextureCoords[0];
		for(u32 m = 0; m < submesh.mNumVertices; m++, vecPtr++) *vecPtr = aiVector2D(texcoord[i].x, texcoord[i].y);
	}

	field.Type = GL_FLOAT;
	field.Index = 0;
	field.Name = name;
	field.TypeCount = 2;
}

template<>
void CreateField<FORMAT_TRIANGLES_BASIC>(u8 count, aiMesh** mesh, size_t offset, gETF::VertexField& field, const char* name)
{
	field.Count = 0;
	for(u8 i = 0; i < count; i++) field.Count += mesh[i]->mNumFaces;

	field.Data = new aiVector3t<u32>[field.Count];
	auto* vecPtr = (aiVector3t<u32>*) field.Data;

	for(u8 i = 0; i < count; i++)
	{
		aiMesh& submesh = *mesh[i];
		aiFace* faces = *(aiFace**)((u8*) &submesh + offset);

		for(u32 m = 0; m < submesh.mNumFaces; m++, vecPtr++)
			memcpy(vecPtr, faces[m].mIndices, sizeof(aiVector3t<u32>));
	}

	field.Type = GL_UNSIGNED_INT;
	field.Index = 0;
	field.Name = name;
	field.TypeCount = 3;
}

int main()
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("cube.glb", IMPORT_FLAGS);

	auto* submeshCount = new u8[scene->mNumMeshes] {};
	u8 realMeshCount = 0;
	const aiMesh* previousMesh = nullptr;

	for(u8 i = 0; i < (u8) MAX(scene->mNumMeshes, UINT8_MAX); i++)
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

		mesh.TriangleMode = TRIANGLE_MODE_SIMPLE;

		mesh.FieldCount = 5; // POS UV NOR TAN TRI
		mesh.Fields = new gETF::VertexField[4];


		CreateField<FORMAT_VEC3>(submeshCount[i], sourceMesh, offsetof(aiMesh, mVertices), mesh.Fields[0], "POS");
		CreateField<FORMAT_TEXCOORD>(submeshCount[i], sourceMesh, offsetof(aiMesh, mTextureCoords), mesh.Fields[1], "UV");
		CreateField<FORMAT_VEC3>(submeshCount[i], sourceMesh, offsetof(aiMesh, mNormals), mesh.Fields[2], "NOR");
		CreateField<FORMAT_VEC3>(submeshCount[i], sourceMesh, offsetof(aiMesh, mTangents), mesh.Fields[3], "TAN");
		CreateField<FORMAT_TRIANGLES_BASIC>(submeshCount[i], sourceMesh, offsetof(aiMesh, mFaces), mesh.Fields[4], "TRI");

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

	return 0;
}
