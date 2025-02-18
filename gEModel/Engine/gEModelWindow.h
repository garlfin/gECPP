//
// Created by scion on 11/14/2024.
//

#pragma once

#include <Window.h>

namespace gE::gEModel
{
    class gEModelWindow : public Window
    {
    public:
        gEModelWindow() : Window(glm::u16vec2(1920, 1080)) {}

        SET_XVAL(std::string, Input, _input);
        SET_XVAL(std::string, Output, _output);
        GET(Array<Mesh>&, Meshes, _meshes);

    protected:
        void OnRender(float) override;
        void OnUpdate(float) override {};

    private:
        std::string _input;
        std::string _output;
        Array<Mesh> _meshes;
    };
}
