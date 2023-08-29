//
// Created by scion on 8/9/2023.
//

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "DemoWindow.h"

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
}

