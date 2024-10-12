//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "Demo/Engine/Entity/FlyCamera.h"
#include "Demo/Engine/Entity/StaticMeshEntity.h"
#include "Engine/Renderer/PBRMaterial.h"

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

	Reference<API::IndexedVAO> cube = ref_create<API::IndexedVAO>();
	ReadSerializableFromFile(this, "Resource/Model/Plane.001.vao", *cube);

	auto* mesh = new StaticMeshEntity(this, cube);
	mesh->GetTransform().SetScale() = glm::vec3(0.5);
	mesh->GetRenderer().SetMaterial(0, cobbleMaterial);
	mesh->GetRenderer().SetMaterial(1, tileMaterial);

	mesh = new StaticMeshEntity(this, cube);
	mesh->GetTransform().SetPosition() = glm::vec3(0, -10, 0);
	mesh->GetRenderer().SetMaterial(0, tileMaterial);
	mesh->GetRenderer().SetMaterial(1, cobbleMaterial);

	glm::vec3 sunRotation(-31.f, 30.f, 0.f);
	auto* sun = new DirectionalLight(this, 1024, 10.f, glm::quat(radians(sunRotation)));
	Lights->Sun = sun;

	auto* camera = new FlyCamera(this);
	Cameras.CurrentCamera = &camera->GetTarget();

	auto* cubemapCap = new CubemapCapture(this, 512);
	cubemapCap->GetTransform().SetPosition().y = 2.1f;
	cubemapCap->GetTransform().SetScale() = glm::vec3(2.1f);

	Cubemaps->Skybox = ref_cast((GL::TextureCube*) PVR::Read(this, "Resource/Texture/sky.pvr", GPU::WrapMode::Clamp));

	VoxelSceneCapture = gE::ptr_create<VoxelCapture>(this, 128, 4.2f);
}