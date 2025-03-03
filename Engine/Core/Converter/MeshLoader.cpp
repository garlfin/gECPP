//
// Created by scion on 11/14/2024.
//


#include "MeshLoader.h"
#include "MeshUtility.h"

#include <filesystem>
#include <Core/Mesh/Mesh.h>
#include <Core/Mesh/Skeleton.h>
#include <Graphics/Buffer/VAO.h>
#include <Vendor/ASSIMP/Importer.hpp>
#include <Vendor/ASSIMP/postprocess.h>
#include <Vendor/ASSIMP/scene.h>

using pp = aiPostProcessSteps;

CONSTEXPR_GLOBAL unsigned POST_PROCESS =
	aiProcess_Triangulate | aiProcess_FindInvalidData | aiProcess_OptimizeMeshes |
	aiProcess_ImproveCacheLocality | aiProcess_FindInstances |
	aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_PopulateArmatureData;

namespace gE::Model
{
	void Read(Window* window, const std::filesystem::path& path, Array<Mesh>& meshesOut, Array<Skeleton>& skeletons)
	{
		GE_ASSERT(window);

		Assimp::Importer importer = DEFAULT;
		const aiScene& scene = *importer.ReadFile(path.string(), POST_PROCESS);

		aiMesh* previousMesh = nullptr;
		std::vector<std::vector<aiMesh*>> meshesIn;
		for(unsigned i = 0; i < scene.mNumMeshes; i++)
		{
			aiMesh* mesh = scene.mMeshes[i];

			if(!previousMesh || mesh->mName != previousMesh->mName)
				meshesIn.emplace_back().push_back(mesh);
			else
				meshesIn.back().push_back(mesh);

			previousMesh = mesh;
		}

		skeletons = Array<Skeleton>(1);
		Skeleton& skeleton = skeletons[0];

		size_t boneCount = 0;
		IterateSkeleton<size_t, IterateBoneCount>(*scene.mRootNode, boneCount);

		if(boneCount)
		{
			skeleton.Bones = Array<Bone>(boneCount);

			Bone* currentBone = skeleton.Bones.Data();
			IterateSkeleton<Bone*, ConvertBone>(*scene.mRootNode, currentBone);

			/*Path path = out / source.filename().replace_extension(".skel");
			WriteSerializableToFile(path, skeleton);

			std::cout << "Wrote skeleton to " << path << std::endl;*/
		}

		meshesOut = Array<Mesh>(meshesIn.size());
		for(size_t i = 0; i < meshesIn.size(); i++)
		{
			Mesh& mesh = meshesOut[i];
			const std::vector<aiMesh*>& src = meshesIn[i];

			GPU::IndexedVAO meshSettings;
			ConvertMesh(src, meshSettings);

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

			mesh.VAO = ptr_cast<API::IVAO>(new API::IndexedVAO(window, move(meshSettings)));

			/*if(hasSkeleton)
			{
				GPU::Buffer<VertexWeight> weightSettings;
				ConvertBones(src, skeleton, weightSettings);

				mesh.BoneWeights = ptr_create<API::Buffer<VertexWeight>>(window, move(weightSettings));
			}*/
		}
	}

	void ReadAsFile(Window* window, const std::filesystem::path& path, Array<File>& files)
	{
		Array<Mesh> meshes;
		Array<Skeleton> skeletons;
		Read(window, path, meshes, skeletons);

		files = Array<File>(meshes.Count());

		Path parentDirectory = path.parent_path();

		size_t fileOffset = 0;
		for(size_t i = 0; i < meshes.Count(); i++, fileOffset++)
		{
			Mesh& mesh = meshes[i];
			Path filePath = parentDirectory / Path(mesh.Name).replace_extension(Mesh::Type.Extension);

			files[fileOffset] = File(window, filePath, std::move(mesh));
		}
	}
}
