//
// Created by scion on 4/24/2025.
//

#include "VR.h"
#include <Core/Log.h>
#include <Core/Math/Math.h>
#include <OpenVR/openvr_mingw.h>

#include "Window.h"
#include "Component/Camera/Camera.h"

namespace gE
{
    VR::VR(Window* window) : _window(window)
    {
        vr::HmdError error;
        _system = VR_Init(&error, vr::EVRApplicationType::VRApplication_Scene);
        _compositor = vr::VRCompositor();
        _devices = new vr::TrackedDevicePose_t[vr::k_unMaxTrackedDeviceCount];

        if(error)
            Log::Error(std::format("Failed to initialize OpenVR. Error code {}.", (u32) error));
        if(!_compositor)
            Log::Error(std::format("Failed to initialize OpenVR Compositor."));

        window->GetShaderCompilationState().emplace_back("VR");
    }

    void VR::OnUpdate() const
    {
        if(!_camera) return;

        float fSecondsSinceLastVSync;
        _system->GetTimeSinceLastVsync(&fSecondsSinceLastVSync, nullptr);

        const float displayFrequency = _system->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
        const float frameDuration = 1.f / displayFrequency;
        const float vSyncToPhotons = _system->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);

        const float fPredictedSecondsFromNow = frameDuration - fSecondsSinceLastVSync + vSyncToPhotons;

        _compositor->WaitGetPoses(_devices, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
       // _system->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, fPredictedSecondsFromNow, _devices, vr::k_unMaxTrackedDeviceCount);

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
        return ToGE(_system->GetEyeToHeadTransform(vr::Eye_Left));
    }

    mat4 VR::GetRightEyeMatrix() const
    {
        return ToGE(_system->GetEyeToHeadTransform(vr::Eye_Right));
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
