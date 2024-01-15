//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "gEModel/gETF/File.h"
#include "Demo/Engine/Entity/FlyCamera.h"
#include "Demo/Engine/Entity/StaticMeshEntity.h"
#include "Engine/Renderer/PBRMaterial.h"

using namespace VoxelDemo;

void DemoWindow::OnInit()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_MULTISAMPLE);

	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glClearColor(0.2, 0.2, 1, 1);

	auto albedo = gE::ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_col.pvr"));
	auto amr = gE::ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_amr.pvr"));
	auto normal = gE::ref_cast((GL::Texture2D*) PVR::Read(this, "Resource/Texture/tile_nor.pvr"));
	gE::PBRMaterialSettings materialSettings { albedo, amr, normal };

	auto rasterShader = gE::ref_create<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	auto rasterMaterial = gE::ref_create<gE::PBRMaterial>(this, rasterShader, materialSettings);

	gETF::File cube;
	gETF::Read("Resource/Model/cube.gETF", cube);

	auto* mesh = new VoxelDemo::StaticMeshEntity(this, cube.Meshes[0]);

	mesh->GetTransform().Scale = glm::vec3(0.5);
	mesh->GetMaterials().SetMaterial(0, rasterMaterial);

	glm::vec3 sunRotation(-31.f, 30.f, 0.f);
	auto* sun = new gE::DirectionalLight(this, 1024, 10.f, glm::quat(glm::radians(sunRotation)));
	Lights.Sun = sun;

	auto* camera = new FlyCamera(this);
	Cameras.CurrentCamera = &camera->GetTarget();

	auto* cubemapCap = new gE::CubemapCapture(this, 512);
	cubemapCap->GetTransform().Position.y = 4.1f;
	cubemapCap->GetTransform().Scale = glm::vec3(2.1f);

	Cubemaps.Skybox = gE::ref_cast((GL::TextureCube*) PVR::Read(this, "Resource/Texture/sky.pvr", GL::WrapMode::Clamp));

	VoxelSceneCapture = new gE::VoxelCapture(this, 128, 4.2f);
}