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

void DemoWindow::OnUpdate(float delta)
{
	Behaviors.OnUpdate(delta);
}

void DemoWindow::OnRender(float delta)
{
	Transforms.OnRender(delta);
	Behaviors.OnRender(delta);

	Lights.OnRender(delta);
	Cameras.OnRender(delta);

	GE_ASSERT(Cameras.CurrentCamera, "CAMERA SHOULD NOT BE NULL!");

	GL::FrameBuffer::Reset();
	Blit(*Cameras.CurrentCamera->GetAttachment(0));
}

void DemoWindow::OnInit()
{
	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glClearColor(0.2, 0.2, 1, 1);

	gE::Reference<GL::Texture2D> tex = gE::CreateReferenceFromPointer((GL::Texture2D*) PVR::Read(this, "../x.pvr"));
	gE::PBRMaterialSettings materialSettings { tex };

	auto rasterShader = gE::CreateReference<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	auto rasterMaterial = gE::CreateReference<gE::PBRMaterial>(this, rasterShader, materialSettings);

	gETF::File file;
	gETF::Read("cube.gETF", file);

	auto* mesh = new VoxelDemo::StaticMeshEntity(this, file.Meshes[0]);
	mesh->GetMaterials().SetMaterial(0, std::move(rasterMaterial));

	glm::vec3 sunRotation(-45.f, 23.f, 0.f);
	auto* sun = new gE::DirectionalLight(this, 1024, 3.f, glm::quat(sunRotation));
	Lights.Sun = sun;

	auto* camera = new FlyCamera(this);
	Cameras.CurrentCamera = &camera->GetCamera();
}

void DemoWindow::OnDestroy()
{
}

