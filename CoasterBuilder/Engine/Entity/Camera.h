//
// Created by scion on 4/10/2025.
//

#pragma once

#include <CoasterBuilder/Engine/Component/CameraMovement.h>
#include <Component/Camera/Camera.h>
#include <Core/Pipeline/DefaultPipeline.h>
#include <Core/Pipeline/PostProcess/Tonemap.h>
#include <Entity/Entity.h>

#include "Core/Pipeline/PostProcess/TAA.h"

namespace Coaster
{
    class FreeCamera final : public Entity
    {
        REFLECTABLE_ONGUI_PROTO(Entity);

    public:
        explicit FreeCamera(Window* window) : Entity(window),
            _taaSettings{ &DefaultPipeline::ColorFormat, &_renderTarget.GetVelocity(), &_renderTarget.GetPreviousDepth() },
            _camera(this, _renderTarget, GetCameraSettings(window), &window->GetCameras()),
            _renderTarget(*this, _camera, { &_exposure, &_tonemap, &_taa }),
            _tonemap(&_renderTarget, &_physicalCamera),
            _exposure(&_renderTarget, &_physicalCamera),
            _taa(&_renderTarget, &_taaSettings),
            _movement(this)
        {}

        GET(gE::Camera2D&, Camera, _camera);
        GET(DefaultPipeline::Target2D&, RenderTarget, _renderTarget)
        GET(FreeCamMovement&, Movement, _movement);

    private:
        static PerspectiveCameraSettings GetCameraSettings(Window* window);

        PostProcess::PhysicalCameraSettings _physicalCamera = DEFAULT;
        PostProcess::TAASettings _taaSettings;

        PerspectiveCamera _camera;
        DefaultPipeline::Target2D _renderTarget;
        PostProcess::Tonemap _tonemap;
        PostProcess::AutoExposure _exposure;
        PostProcess::TAA _taa;
        FreeCamMovement _movement;
    };

    inline REFLECTABLE_ONGUI_IMPL(FreeCamera,
        DrawField(Field{ "Camera" }, _camera, depth);
        DrawField(Field{ "Movement" }, _movement, depth);
        DrawField(Field{ "Physical Camera Settings" }, _physicalCamera, depth);
    );

    inline PerspectiveCameraSettings FreeCamera::GetCameraSettings(Window* window)
    {
        PerspectiveCameraSettings settings
        {
            CameraSettings2D
            {
                ICameraSettings
                {
                    vec2(0.01f, 200.f),
                    DEFAULT
                },
                window->GetSize(),
            },
            80.f,
            AngleType::Degree
        };

        return settings;
    }
}
