//
// Created by scion on 9/5/2023.
//

#include "Camera.h"

#include <Window.h>
#include <Component/Transform.h>
#include <glm/gtx/string_cast.hpp>

#define NOT(EXPR) (!(EXPR))

CONSTEXPR_GLOBAL vec3 ForwardDirs[]
{
    vec3(1, 0, 0),
    vec3(-1, 0, 0),
    vec3(0, 1, 0),
    vec3(0, -1, 0),
    vec3(0, 0, 1),
    vec3(0, 0, -1)
};

CONSTEXPR_GLOBAL vec3 UpDirs[]
{
    vec3(0, -1, 0),
    vec3(0, -1, 0),
    vec3(0, 0, 1),
    vec3(0, 0, -1),
    vec3(0, -1, 0),
    vec3(0, -1, 0)
};

namespace gE
{
    Camera::Camera(Entity* p, IRenderTarget& t, const ICameraSettings& s, ComponentManager<Camera>* m) :
        Component(p, m),
        Projection(),
        _settings(s),
        _target(t)
    {
    }

    void Camera::GetGPUCamera(GPU::Camera& cam)
    {
        const Transform& transform = GetOwner().GetTransform();

        cam.Position[0] = vec4(transform.GetGlobalTransform().Position, 0.f);
        cam.Frame = Frame;
        cam.Planes = GetClipPlanes();
        cam.Size = GetViewportSize();
        cam.Projection = Projection;
        cam.FrameDelta = GetWindow().GetFrameDelta();

        cam.DepthTexture = (handle)0u;
        cam.ColorTexture = (handle)0u;
    }

    void Camera::OnRender(float delta, Camera* callingCamera)
    {
        UpdateProjection();

        DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();
        GPU::Camera& camera = **buffers.GetCamera().GetData();

        bool isFirst = _settings.Timing.GetIsFirst();
        bool shouldTick = _settings.Timing.Tick();

        if (!isFirst && (!shouldTick || GetOwner().GetFlags().Static)) return;

        // Limits "recursion"
        if (!_target->Setup(delta, callingCamera)) return;
        _target->RenderDependencies(delta);

        GetGPUCamera(camera);
        _target->GetGPUCameraOverrides(camera);

        buffers.GetCamera().UpdateData();

        _target->Bind();
        _target->RenderPass(delta, callingCamera);
        _target->PostProcessPass(delta);

        Frame++;
    }

    REFLECTABLE_ONGUI_IMPL(Camera,
    {
        DrawField(ScalarField{"Clip Planes", "", 0.01f, 1000.f, 0.1f}, _settings.ClipPlanes, depth);
        DrawField(ScalarField<u8>{"Tick Offset", "First frame that renders."}, _settings.Timing.TickOffset, depth);
        DrawField(ScalarField<u8>{"Tick Skip", "ie. 0 = every frame, 1 = every other"}, _settings.Timing.TickSkip, depth);
        DrawField<const u32>(ScalarField<u32>{"Frame"}, Frame, depth);
    });

    Camera2D::Camera2D(Entity* p, TARGET_T& t, const CameraSettings2D& s, ComponentManager<Camera>* m) :
        Camera(p, t, s, m),
        _size(s.Size)
    {
    }

    void Camera::SetViewport() const
    {
        const Size2D size = GetViewportSize();
        glViewport(0, 0, size.x, size.y);
    }

    REFLECTABLE_ONGUI_IMPL(Camera2D,
    {
        DrawField<const float>(ScalarField<float>{"Aspect"}, GetAspect(), depth);
        DrawField(ScalarField{ "Resolution", "", 1u }, *this, depth, GetSize, Resize);
    });

    void Camera2D::GetGPUCamera(GPU::Camera& camera)
    {
        Camera::GetGPUCamera(camera);

        camera.View[0] = inverse(GetOwner().GetTransform().Model());
        camera.PreviousView[0] = inverse(GetOwner().GetTransform().PreviousRenderModel());
    }

    void Camera2D::Resize(Size2D size)
    {
        _size = size;
        GetTarget().Resize();
    }

    PerspectiveCamera::PerspectiveCamera(Entity* p, TARGET_T& t, const PerspectiveCameraSettings& s, ComponentManager<Camera>* m) :
        Camera2D(p, t, s, m)
    {
        SetFOV(s.FOV);
    }

    void PerspectiveCamera::GetGPUCamera(GPU::Camera& camera)
    {
        Camera2D::GetGPUCamera(camera);

        if(_isVR)
        {
            const VR& vr = *GetWindow().GetVR();

            camera.View[1] = camera.View[0] * vr.GetRightEyeMatrix();
            camera.View[0] = camera.View[1] * vr.GetLeftEyeMatrix();

            TransformData view = Decompose(inverse(camera.View[0]));
            camera.Position[0] = vec4(view.Position, 0.f);

            view = Decompose(inverse(camera.View[1]));
            camera.Position[1] = vec4(view.Position, 1.f);
        }

        camera.Parameters.x = GetFOV<AngleType::Radian>();
    }

    void PerspectiveCamera::OnRender(float delta, Camera* callingCamera)
    {
        if(_isVR) Resize(GetWindow().GetVR()->GetSize());
        Camera2D::OnRender(delta, callingCamera);
    }

    void PerspectiveCamera::UpdateProjection()
    {
        if(_isVR)
            Projection = GetWindow().GetVR()->GetProjectionMatrix(GetClipPlanes());
        else
            Projection = perspectiveFov(_fov, (float)GetSize().x, (float)GetSize().y, GetClipPlanes().x,GetClipPlanes().y);
    }

    REFLECTABLE_ONGUI_IMPL(PerspectiveCamera,
    {
        float fovDeg = GetFOV();
        if (DrawField(ScalarField{"FOV", "Vertical FOV in Degrees", 1.f, 120.f, 1.f}, fovDeg, depth))
            SetFOV(fovDeg);
    });

    REFLECTABLE_FACTORY_NO_IMPL(PerspectiveCamera);

    OrthographicCamera::OrthographicCamera(Entity* p, TARGET_T& t, const OrthographicCameraSettings& s, ComponentManager<Camera>* m) :
        Camera2D(p, t, s, m),
        _orthographicScale(s.Scale)
    {
    }

    void OrthographicCamera::UpdateProjection()
    {
        const vec4 scale = vec4(-_orthographicScale, _orthographicScale) / 2.f;
        Projection = ortho(scale.x, scale.z, scale.y, scale.w, GetClipPlanes().x, GetClipPlanes().y);
    }

    REFLECTABLE_ONGUI_IMPL(OrthographicCamera,
    {
        DrawField(ScalarField{"Orthographic Scale", "", 0.01f}, _orthographicScale, depth);
    });

    REFLECTABLE_FACTORY_NO_IMPL(OrthographicCamera);

    void CameraCube::Resize(Size1D size)
    {
        _size = size;
        GetTarget().Resize();
    }

    Camera3D::Camera3D(Entity* p, TARGET_T& t, const CameraSettings3D& s, ComponentManager<Camera>* m) :
        Camera(p, t, s, m),
        _size(s.Size)
    {
    }

    void Camera3D::Resize(Size3D size)
    {
        _size = size;
        GetTarget().Resize();
    }

    void Camera3D::UpdateProjection()
    {
        vec3 scale = GetOwner().GetTransform()->Scale;
        Projection = ortho(-scale.x, scale.x, -scale.z, scale.z, 0.01f, scale.y * 2.f);
    }

    REFLECTABLE_ONGUI_IMPL(Camera3D,
    {
        DrawField(ScalarField{ "Resolution", "", 1u }, *this, depth, &Camera3D::GetSize, &Camera3D::Resize);
    });

    REFLECTABLE_FACTORY_NO_IMPL(Camera3D);

    void Camera3D::GetGPUCamera(GPU::Camera& cam)
    {
        Camera::GetGPUCamera(cam);

        const vec3 scale = GetOwner().GetTransform()->Scale;
        const vec3 pos = GetOwner().GetTransform().GetGlobalTransform().Position;

        for (u8 i = 0; i < 3; i++)
            cam.View[i] = lookAt(pos - scale * ForwardDirs[i * 2], pos, UpDirs[i * 2]);
    }

    CameraCube::CameraCube(Entity* p, TARGET_T& t, const CameraSettings1D& s, ComponentManager<Camera>* m) :
        Camera(p, t, s, m),
        _size(s.Size)
    {
    }

    REFLECTABLE_ONGUI_IMPL(CameraCube,
    {
        DrawField(ScalarField{ "Resolution", "", 1u }, *this, depth, &CameraCube::GetSize, &CameraCube::Resize);
    });

    REFLECTABLE_FACTORY_NO_IMPL(CameraCube);

    void CameraCube::UpdateProjection()
    {
        Projection = perspectiveFov(radians(90.f), 1.f, 1.f, GetClipPlanes().x, GetClipPlanes().y);
    }

    void CameraCube::GetGPUCamera(GPU::Camera& cam)
    {
        Camera::GetGPUCamera(cam);

        const vec3 pos = GetOwner().GetTransform().GetGlobalTransform().Position;
        for (u8 i = 0; i < 6; i++)
            cam.View[i] = lookAt(pos, pos + ForwardDirs[i], UpDirs[i]);
    }
}
