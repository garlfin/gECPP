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
	Cameras.OnRender(delta);

	GL::FrameBuffer::Reset();
	Blit(*Cameras.GetCurrentCamera()->GetAttachment(0));
}

void DemoWindow::OnInit()
{
	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glClearColor(0.2, 0.2, 1, 1);

	gE::PBRMaterialSettings materialSettings { PVR::Read(this, "../../x.pvr") };

	auto rasterShader = gE::CreateReference<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	Array<gE::Reference<gE::Material>> materials { 1 };
	materials[0] = gE::CreateReference<gE::PBRMaterial>(this, rasterShader, materialSettings);

	gETF::File file;
	gETF::Read("cube.gETF", file);

	new VoxelDemo::StaticMeshEntity(this, file.Meshes[0], materials);

	auto* camera = new FlyCam(this);
	Cameras.SetCurrentCamera(&camera->GetCamera());
}

void DemoWindow::OnDestroy()
{

}

