//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "gEModel/gETF/File.h"
#include "GL/Buffer/Mesh.h"
#include "Demo/Engine/Entity/FlyCamera.h"

using namespace VoxelDemo;

void DemoWindow::OnUpdate(float delta)
{
	Behaviors.OnUpdate(delta);
}

void DemoWindow::OnRender(float delta)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Transforms.OnRender(delta);
	Cameras.OnRender(delta);
}

void DemoWindow::OnInit()
{
	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glClearColor(0.2, 0.2, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	PipelineBuffers = new gE::DefaultPipelineBuffers(this);

	gE::CreateHandle<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag")->Bind();
	GL::TextureSettings<GL::TextureDimension::D3D> voxelTexSettings{{64, 64, 64}, GL_RGBA16F};
	gE::CreateHandle<GL::Texture3D>(this, voxelTexSettings)->Bind(0, GL_WRITE_ONLY);

	gETF::Header file;
	gETF::Read("cube.gETF", file);
	_testHandle = gE::CreateHandleFromPointer<GL::VAO>(GL::VAO::Create(this, GL::Mesh(file.Meshes[0])));


	new FlyCam(this);

	GL::Scene test{1, glm::mat4(1.f)};
	test.Normal[0] = glm::mat3(1.f);

	PipelineBuffers->Scene.ReplaceData(&test);
}

void DemoWindow::OnDestroy()
{

}

