#include "DemoWindow.h"

int main()
{
	gE::VoxelDemo::DemoWindow window(glm::u16vec2(1280, 720));
	window.Run();
}