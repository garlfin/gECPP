#include "Engine/DemoWindow.h"

int main(int argc, char** argv)
{
#ifdef DEBUG
	gE::OverrideSTDTerminate();
#endif

	bool restart = true;
	while(restart)
	{
		gE::VoxelDemo::DemoWindow window(glm::u16vec2(1920, 1080));
		restart = window.Run();
	}

	return 0;
}
