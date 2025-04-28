//
// Created by scion on 4/24/2025.
//

#pragma once

#include <Prototype.h>
#include <Component/Camera/Camera.h>
#include <Core/Macro.h>
#include <Graphics/Buffer/VAO.h>

namespace gE {
    class PerspectiveCamera;
}

namespace vr
{
    class IVRSystem;
    class IVRCompositor;
    class IVRInput;
    class TrackedDevicePose_t;
    class HmdMatrix34_t;
    class HmdMatrix44_t;
}

namespace gE
{
    mat4 ToGE(const vr::HmdMatrix34_t& m);
    mat4 ToGE(const vr::HmdMatrix44_t& m);

    using ActionHandle = u64;

    class VRControllerState final : public IControllerState
    {
    public:
        explicit VRControllerState(Window* window);

        void Update() override;

        GET_CONST(const TransformData&, LeftControllerTransform, _leftControllerTransform);
        GET_CONST(const TransformData&, RightControllerTransform, _rightControllerTransform);

        GET_CONST(Window&, Window, *_window);

        ~VRControllerState() override = default;

    private:
        vec3 GetAnalog(ActionHandle) const;
        void GetDigital(ActionHandle, KeyState& keyState) const;

        Window* _window;
        ActionHandle _actionSetHandle;

        TransformData _leftControllerTransform;
        TransformData _rightControllerTransform;

        ActionHandle _leftControllerTransformHandle;
        ActionHandle _rightControllerTransformHandle;

        ActionHandle _leftTriggerHandle;
        ActionHandle _rightTriggerHandle;

        ActionHandle _leftBumperHandle;
        ActionHandle _rightBumperHandle;

        ActionHandle _leftStickHandle;
        ActionHandle _leftStickButtonHandle;
        ActionHandle _rightStickHandle;
        ActionHandle _rightStickButtonHandle;

        ActionHandle _aButtonHandle;
        ActionHandle _bButtonHandle;
        ActionHandle _xButtonHandle;
        ActionHandle _yButtonHandle;
    };

    class VR
    {
    public:
        explicit VR(Window* window);

        void OnUpdate() const;
        void OnRender() const;

        GET_CONST(Window&, Window, *_window);
        GET_CONST(const IColorTarget*, CurrentCamera, _camera);
        GET_CONST(const auto&, LeftEyeStencil, _leftEyeStencil);
        GET_CONST(const auto&, RightEyeStencil, _rightEyeStencil);

        void SetCurrentCamera(const ColorTarget<Camera2D>*);

        mat4 GetLeftEyeMatrix() const;
        mat4 GetRightEyeMatrix() const;
        mat4 GetProjectionMatrix(vec2 planes) const;
        Size<Dimension::D2D> GetSize() const;
        VRControllerState& GetController() const;

        static bool IsVRHeadsetPresent();

        ~VR();

        friend class VRControllerState;

    private:
        Window* _window;
        vr::IVRSystem* _system;
        vr::IVRCompositor* _compositor;
        vr::TrackedDevicePose_t* _devices;

        API::VAO _leftEyeStencil;
        API::VAO _rightEyeStencil;

        const IColorTarget* _camera = DEFAULT;
    };
}
