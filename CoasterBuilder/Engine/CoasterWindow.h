//
// Created by scion on 4/10/2025.
//

#pragma once

#include <Window.h>

namespace Coaster
{
    using namespace gE;

    class CoasterBuilderWindow : public Window
    {
    public:
        explicit CoasterBuilderWindow(u16vec2 size, const char* name = "Roller Coaster Builder") : Window(size, name) {};

    private:
        void OnInit() override;
    };
}
