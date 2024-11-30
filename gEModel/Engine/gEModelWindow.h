//
// Created by scion on 11/14/2024.
//

#pragma once

#include <../../gECPP/Engine/Window/Window.h>

namespace gE::gEModel
{
    class gEModelWindow : public Window
    {
    public:
        gEModelWindow() : Window(glm::u16vec2(215, 215))
        {

        }

    protected:
        void OnRender(float) override {};
        void OnInit() override;
        void OnUpdate(float) override;
    };
}
