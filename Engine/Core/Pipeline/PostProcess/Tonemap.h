//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Component/Camera/PostProcessEffect.h>
#include <Core/Pipeline/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>

#include "PhysicalCamera.h"

#define TONEMAP_GROUP_SIZE 8

namespace gE::PostProcess
{
    class AutoExposure final : public PostProcessEffect<RenderTarget<Camera2D>, PhysicalCameraSettings>
    {
        POSTPROCESS_CONSTRUCTOR(AutoExposure);

    public:
        NODISCARD bool RenderPass(GL::Texture2D& in, GL::Texture2D& out) override;
        void Resize() override {};
    };

    class Tonemap final : public PostProcessEffect<RenderTarget<Camera2D>, PhysicalCameraSettings>
    {
        POSTPROCESS_CONSTRUCTOR(Tonemap);

    public:
        NODISCARD bool RenderPass(API::Texture2D& in, API::Texture2D& out) override;
        void Resize() override {};
    };
}
