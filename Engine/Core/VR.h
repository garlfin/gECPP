//
// Created by scion on 4/24/2025.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>

#include "Component/Camera/Camera.h"
#include "Component/Camera/RenderTarget.h"

namespace gE {
    class PerspectiveCamera;
}

namespace vr
{
    class IVRSystem;
    class IVRCompositor;
    class TrackedDevicePose_t;
    class HmdMatrix34_t;
    class HmdMatrix44_t;
}

namespace gE
{
    mat4 ToGE(const vr::HmdMatrix34_t& m);
    mat4 ToGE(const vr::HmdMatrix44_t& m);

    class VR
    {
    public:
        explicit VR(Window* window);

        void OnUpdate() const;
        void OnRender() const;

        GET_CONST(Window&, Window, *_window);
        GET_CONST(const IColorTarget*, CurrentCamera, _camera);

        void SetCurrentCamera(const ColorTarget<Camera2D>*);

        mat4 GetLeftEyeMatrix() const;
        mat4 GetRightEyeMatrix() const;
        mat4 GetProjectionMatrix(vec2 planes) const;
        Size<Dimension::D2D> GetSize() const;

        static bool IsVRHeadsetPresent();

        ~VR();

    private:
        Window* _window;
        vr::IVRSystem* _system;
        vr::IVRCompositor* _compositor;
        vr::TrackedDevicePose_t* _devices;

        const IColorTarget* _camera = DEFAULT;
    };
}
