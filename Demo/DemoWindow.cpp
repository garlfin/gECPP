//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "gEModel/gETF/File.h"
#include "GL/Buffer/Mesh.h"
#include "Entity/MeshRenderer.h"

using namespace VoxelDemo;

void DefaultRenderPass(gE::Window* window, gE::Camera* cam)
{

}


class FlyCam : public gE::Entity
{
 public:
	explicit FlyCam(gE::Window* window) : gE::Entity(window),
		Camera(this, gE::CameraSettings{&window->CameraManager(), {1280, 720}, {0.1f, 100.f}, DefaultRenderPass}, 60)
	{

	}

	gE::PerspectiveCamera Camera;
};

void DemoWindow::OnUpdate(float)
{

}

void DemoWindow::OnRender(float delta)
{
	glClear(GL_COLOR_BUFFER_BIT);

	_testMesh->Draw(0);
}

void DemoWindow::OnInit()
{
	glClearColor(0.2, 0.2, 1, 1);

	PipelineBuffers = new gE::DefaultPipelineBuffers(this);

	_testShader = new GL::Shader(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	_testShader->Bind();

	gETF::Header file;
	gETF::Read("cube.gETF", file);

	FlyCam cam(this);
	cam.Camera.OnRender(0);
	PipelineBuffers->UpdateCamera(cam.Camera);

	GL::Scene test{1, gl::mat4::Identity()};
	PipelineBuffers->Scene.ReplaceData(&test);

	_testMesh = Assets.Register(GL::VAO::Create(this, GL::Mesh(file.Meshes[0])));


}

void DemoWindow::OnDestroy()
{

}

