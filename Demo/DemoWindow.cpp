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

	auto rasterShader = gE::CreateHandle<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	auto rasterMaterial = gE::CreateHandle<gE::Material>(this, rasterShader);

	gETF::File file;
	gETF::Read("cube.gETF", file);

	auto* renderer = new VoxelDemo::StaticMeshEntity(this, file.Meshes[0], rasterMaterial);
	auto* camera = new FlyCam(this);
	Cameras.SetCurrentCamera(&camera->Camera);

	PipelineBuffers->Scene.InstanceCount = 1;
	PipelineBuffers->Scene.Model[0] = glm::mat4(1);
	PipelineBuffers->Scene.Normal[0] = glm::mat3(1);
	PipelineBuffers->UpdateScene(offsetof(GL::Scene, Normal[1]));
}

void DemoWindow::OnDestroy()
{

}

