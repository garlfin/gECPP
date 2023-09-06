//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "gEModel/gETF/File.h"
#include "GL/Buffer/VAOSettings.h"
#include "Entity/MeshRenderer.h"

using namespace VoxelDemo;

void DemoWindow::OnUpdate(float)
{

}

char TITLE_BUF[16];

void DemoWindow::OnRender(float delta)
{
	glClear(GL_COLOR_BUFFER_BIT);

	_testMesh->OnRender(delta);
}

void DemoWindow::OnInit()
{
	glClearColor(0.2, 0.2, 1, 1);

	_testShader = new GL::Shader(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	_testShader->Bind();

	gETF::Header file;
	gETF::Read("cube.gETF", file);

	_testMesh = new gE::MeshRenderer(this, GL::VAO::Create(this, new GL::VAOSettings(file.Meshes[0])));
}

void DemoWindow::OnDestroy()
{

}

