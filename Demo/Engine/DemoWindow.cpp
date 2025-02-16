//
// Created by scion on 8/9/2023.
//

#include <Vendor/GLAD/glad.h>

#include "DemoWindow.h"

#include <Demo/Engine/Entity/Player.h>
#include <Demo/Engine/Entity/StaticMeshEntity.h>
#include <Entity/Light/DirectionalLight.h>
#include <Entity/Light/PointLight.h>
#include <Renderer/Material/PBRMaterial.h>
#include <Demo/Engine/Entity/EmptyColliderEntity.h>
#include <Demo/Engine/Entity/PhysicsCube.h>

using namespace gE::VoxelDemo;

void DemoWindow::OnInit()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_MULTISAMPLE);

	auto albedo = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_col.pvr"));
	auto amr = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_armd.pvr"));
	auto normal = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_nor.pvr"));
	PBRMaterialSettings cobbleSettings { albedo, amr, normal };

	albedo = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_col.pvr"));
	amr = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_armd.pvr"));
	normal = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_nor.pvr"));
	PBRMaterialSettings tileSettings { albedo, amr, normal };

	albedo = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/grass_col.pvr"));
	amr = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/grass_armd.pvr"));
	normal = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/grass_nor.pvr"));
	PBRMaterialSettings grassSettings { albedo, amr, normal };

	auto rasterShader = gE::ref_create<ForwardShader>(this, GPU::Shader("Resource/Shader/uber.vert", "Resource/Shader/uber.frag"));
	auto cobbleMaterial = gE::ref_create<PBRMaterial>(this, rasterShader, cobbleSettings);
	auto tileMaterial = gE::ref_create<PBRMaterial>(this, rasterShader, tileSettings);
	auto grassMaterial = gE::ref_create<PBRMaterial>(this, rasterShader, grassSettings);

	Reference<Mesh> sceneMesh = ref_create<Mesh>();
	ReadSerializableFromFile(this, "Resource/Model/Plane.001.mesh", *sceneMesh);

	Reference<Mesh> cubeMesh = ref_create<Mesh>();
	ReadSerializableFromFile(this, "Resource/Model/Cube.mesh", *cubeMesh);

	auto* mesh = new StaticMeshEntity(this, sceneMesh);
	mesh->SetName("Room");
	mesh->GetTransform().SetScale(glm::vec3(0.5));
	mesh->GetRenderer().SetMaterial(0, cobbleMaterial);
	mesh->GetRenderer().SetMaterial(1, tileMaterial);
	mesh->GetRenderer().SetMaterial(2, grassMaterial);

	glm::vec3 sunRotation(-31.f, 30.f, 0.f);
	auto* sun = new DirectionalLight(this, 1024, 10.f, glm::quat(radians(sunRotation)));
	sun->SetName("Sun");
	Lights->Sun = sun;

	auto* pointLight = new PointLight(this, 512);
	pointLight->SetName("Point Light");
	pointLight->GetTransform().SetPosition(glm::vec3(1.1, 0.5, -1.7));

	auto* player = new Player(this);
	player->SetName("Player");
	player->GetTransform().SetPosition(glm::vec3(0, 10, 0));

	auto* playerCamera = new PlayerCamera(this, *player);
	playerCamera->SetName("Camera");
	Cameras.CurrentCamera = &playerCamera->GetTarget();

	player->GetMovement().SetFPCamera(playerCamera);

	auto* cubemapCap = new CubemapCapture(this, 512);
	cubemapCap->SetName("Cubemap Capture");
	cubemapCap->GetTransform().SetPosition(glm::vec3(0.0, 2.1, 0.0));
	cubemapCap->GetTransform().SetScale(glm::vec3(2.1f));

	auto* physicsCube = new PhysicsCubeEntity(this, cubeMesh, glm::vec3(0.5f));
	physicsCube->SetName("Cube 1");
	physicsCube->GetTransform().SetLocation(glm::vec3(0.f, 6.f, 0.f));
	physicsCube->GetTransform().SetRotation(glm::quat(glm::vec3(32, 7, 22)));
	physicsCube->GetTransform().SetScale(glm::vec3(0.5f));
	physicsCube->GetRenderer().SetMaterial(0, cobbleMaterial);

	physicsCube = new PhysicsCubeEntity(this, cubeMesh, glm::vec3(0.5f));
	physicsCube->SetName("Cube 2");
	physicsCube->GetTransform().SetLocation(glm::vec3(0.f, 5.f, 0.f));
	physicsCube->GetTransform().SetScale(glm::vec3(0.25));

	auto* floor = new EmptyColliderEntity(this, glm::vec3(5.f, 0.1f, 5.f), EntityFlags(true));
	floor->SetName("Floor Collider");
	floor->GetTransform().SetLocation(glm::vec3(0.f, -0.1, 0.f));

	Cubemaps->Skybox = ref_cast((GL::TextureCube*) PVR::Read(this, "Resource/Texture/sky.pvr", GPU::WrapMode::Clamp));
	Cubemaps->Skybox->Free();
	Cubemaps->CreateHarmonic();

	VoxelCaptureSettings voxelSettings(128, 8.4f, VoxelPipeline::ProbeSettings(glm::u8vec3(8)));
	VoxelSceneCapture = gE::ptr_create<VoxelCapture>(this, voxelSettings);
	VoxelSceneCapture->SetName("Voxel Capture");
}