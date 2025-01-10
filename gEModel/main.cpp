//
// Created by scion on 8/10/2023.
//

#include <gEModel/Engine/gEModelWindow.h>

int main(int argc, char** argv)
{
#ifdef DEBUG
    gE::OverrideSTDTerminate();
#endif

    auto window = gE::gEModel::gEModelWindow();
    window.Run();

    return 0;
}

