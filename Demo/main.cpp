#include "DemoWindow.h"

int main()
{
	bool restart = true;
	while(restart)
	{
		gE::VoxelDemo::DemoWindow window(glm::u16vec2(1280, 720));
		restart = window.Run();
	}
}
