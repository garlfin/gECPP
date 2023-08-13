//
// Created by scion on 8/9/2023.
//

#include "GLAD/glad.h"
#include "DemoWindow.h"

using namespace VoxelDemo;

void DemoWindow::OnUpdate(float)
{

}

void DemoWindow::OnRender(float)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void DemoWindow::OnInit()
{
	glClearColor(0.2, 0.2, 1, 1);
}

