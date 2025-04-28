//
// Created by scion on 4/24/2025.
//

#include "VR.h"
#include <Component/Camera/Camera.h>
#include <Core/Log.h>
#include <Core/Math/Math.h>
#include <OpenVR/openvr_mingw.h>

#include "Window.h"

namespace gE
{
    VRControllerState::VRControllerState(Window* window) : _window(window)
    {
        vr::IVRInput* input = vr::VRInput();

        vr::EVRInputError error = input->GetActionHandle("/actions/legacy/in/Left_Pose", &_leftControllerTransformHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/Right_Pose", &_rightControllerTransformHandle);

        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_axis1_value", &_leftTriggerHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_axis1_value", &_rightTriggerHandle);

        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_axis2_press", &_leftBumperHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_axis2_press", &_rightBumperHandle);

        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_axis0_value", &_leftStickHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_axis0_press", &_leftStickButtonHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_axis0_value", &_rightStickHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_axis0_press", &_rightStickButtonHandle);

        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_a_press", &_aButtonHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/right_applicationmenu_press", &_bButtonHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_a_press", &_xButtonHandle);
        if(!error) error = input->GetActionHandle("/actions/legacy/in/left_applicationmenu_press", &_yButtonHandle);

        if(!error) input->GetActionSetHandle("/actions/legacy", &_actionSetHandle);

        if(error)
            Log::Error("Failed to initialize controller bindings.");
    }


    void VRControllerState::Update()
    {
        vr::IVRInput* input = vr::VRInput();

        vr::VRActiveActionSet_t actionSet
        {
            _actionSetHandle,
            vr::k_ulInvalidInputValueHandle
        };

        input->UpdateActionState(&actionSet, sizeof(vr::VRActiveActionSet_t), 1);

        vr::InputPoseActionData_t poseAction;

        input->GetPoseActionDataForNextFrame(_leftControllerTransformHandle, vr::TrackingUniverseStanding, &poseAction, sizeof(vr::InputPoseActionData_t), vr::k_ulInvalidInputValueHandle);
        if(poseAction.pose.bPoseIsValid)
        {
            const mat4 transform = ToGE(poseAction.pose.mDeviceToAbsoluteTracking);
            _leftControllerTransform = Decompose(transform);
        }

        input->GetPoseActionDataForNextFrame(_rightControllerTransformHandle, vr::TrackingUniverseStanding, &poseAction, sizeof(vr::InputPoseActionData_t), vr::k_ulInvalidInputValueHandle);
        if(poseAction.pose.bPoseIsValid)
        {
            const mat4 transform = ToGE(poseAction.pose.mDeviceToAbsoluteTracking);
            _rightControllerTransform = Decompose(transform);
        }

        LeftTrigger = GetAnalog(_leftTriggerHandle).x;
        RightTrigger = GetAnalog(_rightTriggerHandle).x;

        GetDigital(_leftBumperHandle, Buttons[(u8) ControllerButton::LeftBumper]);
        GetDigital(_rightBumperHandle, Buttons[(u8) ControllerButton::RightBumper]);

        LeftStick = GetAnalog(_leftStickHandle);
        GetDigital(_leftStickButtonHandle, Buttons[(u8) ControllerButton::LeftStick]);
        RightStick = GetAnalog(_rightStickHandle);
        GetDigital(_rightStickButtonHandle, Buttons[(u8) ControllerButton::RightStick]);

        GetDigital(_aButtonHandle, Buttons[(u8) ControllerButton::XboxA]);
        GetDigital(_bButtonHandle, Buttons[(u8) ControllerButton::XboxB]);
        GetDigital(_xButtonHandle, Buttons[(u8) ControllerButton::XboxX]);
        GetDigital(_yButtonHandle, Buttons[(u8) ControllerButton::XboxY]);
    }

    vec3 VRControllerState::GetAnalog(ActionHandle action) const
    {
        vr::IVRInput* input = vr::VRInput();

        vr::InputAnalogActionData_t analogAction;
        const vr::EVRInputError error = input->GetAnalogActionData(action, &analogAction, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);

#ifdef DEBUG
        if(error) Log::Error(std::format("OpenVR (GetAnalogActionData) input error: {}", (u32) error));
#endif

        return vec3(analogAction.x, analogAction.y, analogAction.z);
    }

    void VRControllerState::GetDigital(ActionHandle action, KeyState& keyState) const
    {
        vr::IVRInput* input = vr::VRInput();

        vr::InputDigitalActionData_t digitalAction;
        const vr::EVRInputError error = input->GetDigitalActionData(action, &digitalAction, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle);

#ifdef DEBUG
        if(error) Log::Error(std::format("OpenVR (GetDigitalActionData) input error: {}", (u32) error));
#endif

        UpdateKeyState(digitalAction.bState, keyState);
    }

    VR::VR(Window* window) : _window(window)
    {
        vr::HmdError error;
        _system = VR_Init(&error, vr::EVRApplicationType::VRApplication_Scene);
        _compositor = vr::VRCompositor();
        _devices = new vr::TrackedDevicePose_t[vr::k_unMaxTrackedDeviceCount];

        if(error)
            Log::FatalError(std::format("Failed to initialize OpenVR. Error code {}.", (u32) error));
        if(!_compositor)
            Log::FatalError("Failed to initialize OpenVR Compositor.");

        window->GetShaderCompilationState().emplace_back("VR");

        vr::HiddenAreaMesh_t stencilMesh = _system->GetHiddenAreaMesh(vr::Eye_Left);
        GPU::VAO stencilVAO = BlitVAOFormat;
        stencilVAO.AddBuffer(GPU::Buffer(stencilMesh.unTriangleCount * 3 * sizeof(vec2), (const std::byte*) stencilMesh.pVertexData, sizeof(vec2)));
        stencilVAO.Materials[0].Count = stencilMesh.unTriangleCount;
        _leftEyeStencil = API::VAO(window, move(stencilVAO));
        _leftEyeStencil.Free();

        stencilMesh = _system->GetHiddenAreaMesh(vr::Eye_Right);
        stencilVAO = BlitVAOFormat;
        stencilVAO.AddBuffer(GPU::Buffer(stencilMesh.unTriangleCount * 3 * sizeof(vec2), (const std::byte*) stencilMesh.pVertexData, sizeof(vec2)));
        stencilVAO.Materials[0].Count = stencilMesh.unTriangleCount;
        _rightEyeStencil = API::VAO(window, move(stencilVAO));
        _rightEyeStencil.Free();
    }

    void VR::OnUpdate() const
    {
        if(!_camera) return;

        _compositor->WaitGetPoses(nullptr, 0, nullptr, 0);

        float secondsSinceLastVsync;
        _system->GetTimeSinceLastVsync(&secondsSinceLastVsync, nullptr);

        const float displayFrequency = _system->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
        const float frameDuration = 1.f / displayFrequency;
        const float vsyncToPhotons = _system->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);
        const float predictedSecondsFromNow = frameDuration - secondsSinceLastVsync + vsyncToPhotons;

        _system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, predictedSecondsFromNow, _devices, vr::k_unMaxTrackedDeviceCount);

        if(_devices[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
        {
            const mat4 transform = ToGE(_devices[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
            _camera->GetCamera().GetOwner().GetTransform().Set(Decompose(transform));
        }
    }

    void VR::SetCurrentCamera(const ColorTarget<Camera2D>* target)
    {
        if(!target) return;

        GE_ASSERT(dynamic_cast<PerspectiveCamera*>(&target->GetCamera()));

        if(_camera)
            ((PerspectiveCamera&) _camera->GetCamera())._isVR = false;
        _camera = target;
        ((PerspectiveCamera&) target->GetCamera())._isVR = true;
    }

    mat4 VR::GetLeftEyeMatrix() const
    {
        return inverse(ToGE(_system->GetEyeToHeadTransform(vr::Eye_Left)));
    }

    mat4 VR::GetRightEyeMatrix() const
    {
        return inverse(ToGE(_system->GetEyeToHeadTransform(vr::Eye_Right)));
    }

    mat4 VR::GetProjectionMatrix(vec2 planes) const
    {
        return ToGE(_system->GetProjectionMatrix(vr::Eye_Left, planes.x, planes.y));
    }

    Size<Dimension::D2D> VR::GetSize() const
    {
        Size2D result;
        _system->GetRecommendedRenderTargetSize(&result.x, &result.y);
        return result * Size2D(2, 1);
    }

    VRControllerState& VR::GetController() const
    {
        return (VRControllerState&) *GetWindow().GetController();
    }

    void VR::OnRender() const
    {
        if(!_camera) return;

        constexpr vr::VRTextureBounds_t leftEye{ 0.f, 0.f, 0.5f, 1.f };
        constexpr vr::VRTextureBounds_t rightEye{ 0.5f, 0.f, 1.f, 1.f };
        const vr::Texture_t color{ (void*) (u64) _camera->GetColor().Get(), vr::ETextureType::TextureType_OpenGL, vr::ColorSpace_Gamma};

        _compositor->Submit(vr::Eye_Left, &color, &leftEye);
        _compositor->Submit(vr::Eye_Right, &color, &rightEye);
    }

    bool VR::IsVRHeadsetPresent()
    {
        return vr::VR_IsHmdPresent();
    }

    VR::~VR()
    {
        vr::VR_Shutdown();
        delete[] _devices;
    }

    mat4 ToGE(const vr::HmdMatrix34_t& m)
    {
        return mat4
        {
            m.m[0][0], m.m[1][0], m.m[2][0], 0.f,
            m.m[0][1], m.m[1][1], m.m[2][1], 0.f,
            m.m[0][2], m.m[1][2], m.m[2][2], 0.f,
            m.m[0][3], m.m[1][3], m.m[2][3], 1.f
        };
    }

    mat4 ToGE(const vr::HmdMatrix44_t& m)
    {
        return mat4
        {
            m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
            m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
            m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
            m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
        };
    }
}
