//
// Created by scion on 9/5/2023.
//

#pragma once

#include <Core/RelativePointer.h>
#include <Entity/Entity.h>
#include <Graphics/Texture/Texture.h>
#include "RenderTarget.h"
#include "Settings.h"
#include "Timing.h"

namespace GPU
{
    struct Camera;
}

namespace gE
{
    class Camera : public Component
    {
        REFLECTABLE_ONGUI_PROTO(Component);

    public:
        Camera(Entity*, IRenderTarget&, const ICameraSettings&, ComponentManager<Camera>* = nullptr);

        void SetViewport() const;
        void OnInit() override {};
        void OnRender(float delta, Camera* callingCamera) override;

        virtual void GetGPUCamera(GPU::Camera&);
        virtual Size2D GetViewportSize() const = 0;

        GET(IRenderTarget&, Target, *_target);
        GET_CONST(CameraTiming, Timing, _settings.Timing);
        GET_CONST(gE::ClipPlanes, ClipPlanes, _settings.ClipPlanes);
        GET_CONST(const ICameraSettings&, Settings, _settings);
        GET_CONST(const glm::mat4&, Projection, Projection);

    protected:
        virtual void UpdateProjection() = 0;

        glm::mat4 Projection;
        u32 Frame = 0;

    private:
        ICameraSettings _settings;
        RelativePointer<IRenderTarget> _target;
    };

    class Camera2D : public Camera
    {
        REFLECTABLE_ONGUI_PROTO(Camera);

    public:
        using TARGET_T = RenderTarget<Camera2D>;
        using SIZE_T = Size2D;
        using TEX_T = API::Texture2D;

        Camera2D(Entity*, TARGET_T&, const CameraSettings2D&, ComponentManager<Camera>* = nullptr);

        GET(TARGET_T&, Target, (TARGET_T&) Camera::GetTarget());
        GET_CONST(SIZE_T, Size, _size);
        GET_CONST(float, Aspect, (float) GetSize().x / GetSize().y);

        void GetGPUCamera(GPU::Camera& camera) override;
        void Resize(Size2D);
        Size2D GetViewportSize() const override { return _size; }

    private:
        Size2D _size;
    };

    class PerspectiveCamera final : public Camera2D
    {
        REFLECTABLE_PROTO(PerspectiveCamera, Camera2D, "gE::PerspectiveCamera");

    public:
        PerspectiveCamera(Entity*, TARGET_T&, const PerspectiveCameraSettings&, ComponentManager<Camera>* = nullptr);

        template <AngleType T = AngleType::Degree>
        NODISCARD ALWAYS_INLINE float GetFOV() const
        {
            if constexpr (T == AngleType::Radian)
                return _fov;
            return glm::degrees(_fov);
        }

        template <AngleType T = AngleType::Degree>
        ALWAYS_INLINE void SetFOV(float fov)
        {
            if constexpr (T == AngleType::Radian)
                _fov = fov;
            else
                _fov = glm::radians(fov);
        }

        void GetGPUCamera(GPU::Camera& camera) override;

    protected:
        void UpdateProjection() override;

    private:
        float _fov;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(PerspectiveCamera);

    class OrthographicCamera final : public Camera2D
    {
        REFLECTABLE_PROTO(OrthographicCamera, Camera2D, "gE::OrthographicCamera");

    public:
        OrthographicCamera(Entity*, TARGET_T&, const OrthographicCameraSettings&, ComponentManager<Camera>* = nullptr);

        GET_CONST(const glm::vec2&, Scale, _orthographicScale);

    protected:
        void UpdateProjection() override;

    private:
        glm::vec2 _orthographicScale;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(OrthographicCamera);

    class Camera3D final : public Camera
    {
        REFLECTABLE_PROTO(Camera3D, Camera, "gE::Camera3D");

    public:
        using TARGET_T = RenderTarget<Camera3D>;
        using SIZE_T = Size3D;
        using TEX_T = API::Texture3D;

        Camera3D(Entity*, TARGET_T&, const CameraSettings3D&, ComponentManager<Camera>* = nullptr);

        GET(TARGET_T&, Target, (TARGET_T&) Camera::GetTarget());
        GET_CONST(SIZE_T, Size, _size);
        GET_CONST(float, Scale, GetOwner().GetTransform()->Scale.x);

        void GetGPUCamera(GPU::Camera&) override;
        void Resize(Size3D);
        Size2D GetViewportSize() const override { return _size; }

    protected:
        void UpdateProjection() override;

    private:
        Size3D _size;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(Camera3D);

    class CameraCube final : public Camera
    {
        REFLECTABLE_PROTO(CameraCube, Camera, "gE::CameraCube");

    public:
        using TARGET_T = RenderTarget<CameraCube>;
        using SIZE_T = Size1D;
        using TEX_T = API::TextureCube;

        CameraCube(Entity*, TARGET_T&, const CameraSettings1D&, ComponentManager<Camera>* = nullptr);

        GET(TARGET_T&, Target, (TARGET_T&) Camera::GetTarget());
        GET_CONST(SIZE_T, Size, _size);

        void GetGPUCamera(GPU::Camera& camera) override;
        void Resize(Size1D);
        Size2D GetViewportSize() const override { return Size2D(_size); }

    protected:
        void UpdateProjection() override;

    private:
        Size1D _size;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(CameraCube);

    class CameraManager final : public ComponentManager<Camera>
    {
    public:
        using ComponentManager::ComponentManager;

        GET_SET(ColorTarget<Camera2D>*, CurrentCamera, _currentCamera);

        ~CameraManager() override = default;

    private:
        ColorTarget<Camera2D>* _currentCamera = DEFAULT;
    };
}
