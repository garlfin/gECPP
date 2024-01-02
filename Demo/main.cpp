#include "DemoWindow.h"

using namespace VoxelDemo;
using namespace gE;

int main()
{
	DemoWindow window(glm::u16vec2(1280, 720));
	window.Run();
}