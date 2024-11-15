//
// Created by scion on 11/14/2024.
//

#include "gEModelWindow.h"
#include "Utility/MeshLoader.h"

namespace gE::gEModel
{
    void gEModelWindow::OnInit()
    {
        Window::OnInit();

        std::string sourcePath, exportDir;

        std::cout << "Enter source file: ";
        std::cin >> sourcePath;

        std::cout << "Enter destination folder: ";
        std::cin >> exportDir;

        ConvertFile(this, sourcePath, exportDir);
    }

    void gEModelWindow::OnUpdate(float x)
    {
        CloseState = CloseFlags::Close;
    }
}
