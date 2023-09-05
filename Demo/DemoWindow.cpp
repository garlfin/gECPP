//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"
#include "GL/Shader/Shader.h"
#include "gEModel/gETF/File.h"
#include "GL/Buffer/VAOSettings.h"
#include "GL/Buffer/VAO.h"

using namespace VoxelDemo;

void DemoWindow::OnUpdate(float)
{

}

char TITLE_BUF[16];

void DemoWindow::OnRender(float delta)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void DemoWindow::OnInit()
{
	glClearColor(0.2, 0.2, 1, 1);

	GL::Shader shader(this, "Resource/Shader/uber.vert", "Resource/Shader/uber.frag");
	shader.Bind();

	gETF::Header file;
	gETF::Read("cube.gETF", file);
	GL::VAOSettings settings(file.Meshes[0]);

	GL::VAO* vao = GL::VAO::Create(this, &settings);
}

