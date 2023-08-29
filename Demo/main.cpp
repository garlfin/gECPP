#include <iostream>

#include "DemoWindow.h"

using namespace VoxelDemo;
using namespace gE;

int main()
{
	DemoWindow window(gl::u16vec2(1280, 720));
	window.Run();
}
