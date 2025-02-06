//
// Created by scion on 1/27/2025.
//

#pragma once

#include <Component/Camera/PostProcessEffect.h>
#include <Renderer/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>

#define TONEMAP_GROUP_SIZE 8

namespace gE::PostProcess
{
    struct TAASettings
    {
        const GPU::Texture* ColorFormat;
        API::Texture2D* Velocity;
        API::Texture2D* PreviousDepth;
    };

    class TAA final : public PostProcessEffect<RenderTarget<Camera2D>, TAASettings>
    {
    public:
        TAA(TARGET_T* target, TAASettings* settings);

        NODISCARD bool RenderPass(GL::Texture2D& in, GL::Texture2D& out) override;

    private:
        API::Texture2D _taaBack;
    };
}