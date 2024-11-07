//
// Created by scion on 8/9/2023.
//

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"

#include <Demo/Engine/Entity/FlyCamera.h>
#include <Demo/Engine/Entity/StaticMeshEntity.h>
#include <Engine/Entity/Light/DirectionalLight.h>
#include <Engine/Entity/Light/PointLight.h>
#include <Engine/Renderer/PBRMaterial.h>

#include "Engine/Entity/EmptyColliderEntity.h"
#include "Engine/Entity/PhysicsCube.h"

using namespace gE::VoxelDemo;

void DemoWindow::OnInit()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_MULTISAMPLE);

	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	auto albedo = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_col.pvr"));
	auto amr = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_armd.pvr"));
	auto normal = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/cobble_nor.pvr"));
	PBRMaterialSettings cobbleSettings { albedo, amr, normal };

	albedo = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_col.pvr"));
	amr = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_armd.pvr"));
	normal = ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_nor.pvr"));
	PBRMaterialSettings tileSettings { albedo, amr, normal };

	auto rasterShader = gE::ref_create<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	auto cobbleMaterial = gE::ref_create<PBRMaterial>(this, rasterShader, cobbleSettings);
	auto tileMaterial = gE::ref_create<PBRMaterial>(this, rasterShader, tileSettings);

	Reference<API::IndexedVAO> sceneMesh = ref_create<API::IndexedVAO>();
	ReadSerializableFromFile(this, "Resource/Model/Plane.001.vao", *sceneMesh);

	Reference<API::IndexedVAO> cubeMesh = ref_create<API::IndexedVAO>();
	ReadSerializableFromFile(this, "Resource/Model/Cube.001.vao", *cubeMesh);

	auto* mesh = new StaticMeshEntity(this, sceneMesh);
	mesh->GetTransform().SetScale(glm::vec3(0.5));
	mesh->GetRenderer().SetMaterial(0, cobbleMaterial);
	mesh->GetRenderer().SetMaterial(1, tileMaterial);

	mesh = new StaticMeshEntity(this, sceneMesh);
	mesh->GetTransform().SetPosition(glm::vec3(0, -5, 0));
	mesh->GetRenderer().SetMaterial(0, tileMaterial);
	mesh->GetRenderer().SetMaterial(1, cobbleMaterial);

	glm::vec3 sunRotation(-31.f, 30.f, 0.f);
	auto* sun = new DirectionalLight(this, 1024, 10.f, glm::quat(radians(sunRotation)));
	Lights->Sun = sun;

	auto* pointLight = new PointLight(this, 512);
	pointLight->GetTransform().SetPosition(glm::vec3(1.1, 0.5, -1.7));

	auto* camera = new FlyCamera(this);
	Cameras.CurrentCamera = &camera->GetTarget();

	auto* cubemapCap = new CubemapCapture(this, 512);
	cubemapCap->GetTransform().SetPosition(glm::vec3(0.0, 2.1, 0.0));
	cubemapCap->GetTransform().SetScale(glm::vec3(2.1f));

	auto* physicsCube = new PhysicsCubeEntity(this, cubeMesh, glm::vec3(0.5f));
	physicsCube->GetTransform().SetLocation(glm::vec3(0.f, 6.f, 0.f));
	physicsCube->GetTransform().SetRotation(glm::quat(glm::vec3(32, 7, 22)));
	physicsCube->GetTransform().SetScale(glm::vec3(0.5f));
	physicsCube->GetRenderer().SetMaterial(0, cobbleMaterial);

	physicsCube = new PhysicsCubeEntity(this, cubeMesh, glm::vec3(0.5f));
	physicsCube->GetTransform().SetLocation(glm::vec3(0.f, 5.f, 0.f));
	physicsCube->GetTransform().SetScale(glm::vec3(0.25));

	auto* floor = new EmptyColliderEntity(this, glm::vec3(5.f, 0.1f, 5.f), Flags(true));
	floor->GetTransform().SetLocation(glm::vec3(0.f, -0.1, 0.f));

	Cubemaps->Skybox = ref_cast((GL::TextureCube*) PVR::Read(this, "Resource/Texture/sky.pvr", GPU::WrapMode::Clamp));

	VoxelSceneCapture = gE::ptr_create<VoxelCapture>(this, 128, 4.2f);
}