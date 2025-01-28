//
// Created by scion on 1/27/2025.
//

#include "TAA.h"

#include <Engine/Window/Window.h>

#define TAA_GROUP_SIZE 8

namespace gE::PostProcess
{
    TAA::TAA(Window* window, TAASettings const& settings) : PostProcessEffect(window, settings),
        _taaBack(window, *settings.ColorFormat)
    {

    }

    bool TAA::RenderPass(GL::Texture2D& in, GL::Texture2D& out)
    {
        API::ComputeShader& shader = GetWindow().GetTAAShader();

        shader.Bind();

        out.Bind(0, GL_WRITE_ONLY);
        shader.SetUniform(0, in.Use(0));
        shader.SetUniform(1, _taaBack.Use(1));
        shader.SetUniform(2, GetSettings().Velocity->Use(2));
        shader.SetUniform(3, GetSettings().PreviousDepth->Use(3));

        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        shader.Dispatch(DIV_CEIL(in.GetSize(), TAA_GROUP_SIZE));

        _taaBack.CopyFrom(out);

        return true;
    }
}
