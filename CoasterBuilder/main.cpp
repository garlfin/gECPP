#include <CoasterBuilder/Engine/CoasterWindow.h>

int main(int argc, char** argv)
{
#ifdef DEBUG
    gE::OverrideSTDTerminate();
#endif

    bool restart = true;
    while(restart)
    {
        Coaster::CoasterBuilderWindow window(u16vec2(1920, 1080));
        restart = window.Run();
    }
}
