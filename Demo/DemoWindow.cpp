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
	Transforms.OnRender(delta);
	Cameras.OnRender(delta);

	GL::FrameBuffer::Reset();
	Blit(*Cameras[0]->GetAttachment(0));
}

void DemoWindow::OnInit()
{
	glfwSetInputMode(GLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glClearColor(0.2, 0.2, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	PipelineBuffers = new gE::DefaultPipeline::Buffers(this);

	Array<GL::PreprocessorPair> pairs(1);
	pairs[0] = GL::PreprocessorPair("TEST");


	RasterShader = gE::CreateHandle<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	VoxelShader = gE::CreateHandle<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag", &pairs);

	// GL::TextureSettings<GL::TextureDimension::D3D> voxelTexSettings{{64, 64, 64}, GL_RGBA16F};
	// ExportTexture = gE::CreateHandle<GL::Texture3D>(this, voxelTexSettings);

	gETF::Header file;
	gETF::Read("cube.gETF", file);
	Mesh = gE::CreateHandleFromPointer<GL::VAO>(GL::VAO::Create(this, GL::Mesh(file.Meshes[0])));

	new FlyCam(this);


	PipelineBuffers->Scene.InstanceCount = 1;
	PipelineBuffers->Scene.Model[0] = glm::mat4(1);
	PipelineBuffers->Scene.Normal[0] = glm::mat3(1);
	LOG("A");
	PipelineBuffers->UpdateScene(offsetof(GL::Scene, Normal[1]));
}

void DemoWindow::OnDestroy()
{

}

