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
	((GL::VAO*) window->GetAssets()[1])->Draw(0);
}

class Movement : public gE::Behavior
{
 public:
	explicit Movement(gE::Entity* o) :
		gE::Behavior(o),
		_transform(o->GetTransform()), _window(o->GetWindow()->GLFWWindow())
	{

	}

	void OnUpdate(float d) override
	{
		glm::dvec2 mousePos, mouseDelta;
		glfwGetCursorPos(_window, &mousePos.x, &mousePos.y);
		mouseDelta = _prevCursorPos - mousePos;
		_prevCursorPos = mousePos;

		_rot.y += mouseDelta.x * 0.1f;
		_rot.x += mouseDelta.y * 0.1f;
		_rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

		_transform.SetRotation(degree_cast<AngleType::Radian>(_rot));

		glm::vec3 dir(0.f);
		if(glfwGetKey(_window, GLFW_KEY_W)) dir.z -= 1;
		if(glfwGetKey(_window, GLFW_KEY_S)) dir.z += 1;
		if(glfwGetKey(_window, GLFW_KEY_D)) dir.x += 1;
		if(glfwGetKey(_window, GLFW_KEY_A)) dir.x -= 1;

		dir = glm::normalize(dir);
		if(!glm::isinf(dir.x) && !glm::isnan(dir.x)) _transform.Position += _transform.LocalRotationMatrix() * dir * d;
	}

 private:
	gE::Transform& _transform;
	GLFWwindow* const _window;
	glm::dvec2 _prevCursorPos {0.f, 0.f};
	glm::vec3 _rot;
};

class FlyCam : public gE::Entity
{
 public:
	explicit FlyCam(gE::Window* window) : gE::Entity(window),
		Camera(this, gE::CameraSettings{{1280, 720}, {0.1f, 100.f}, DefaultRenderPass}, degree_cast<AngleType::Radian>(80.f)),
	  	_movement(this)
	{

	}

	gE::PerspectiveCamera Camera;
	Movement _movement;
};

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
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	PipelineBuffers = new gE::DefaultPipelineBuffers(this);

	Assets.Create<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag")->Bind();

	gETF::Header file;
	gETF::Read("cube.gETF", file);
	Assets.Register(GL::VAO::Create(this, GL::Mesh(file.Meshes[0])));

	new FlyCam(this);

	GL::Scene test{1, glm::mat4(1.f)};
	test.Normal[0] = glm::mat3(1.f);

	PipelineBuffers->Scene.ReplaceData(&test);
}

void DemoWindow::OnDestroy()
{

}

