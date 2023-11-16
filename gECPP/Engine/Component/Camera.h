//
// Created by scion on 9/5/2023.
//

#pragma once

#include <GL/Texture/Texture.h>
#include <GL/Buffer/FrameBuffer.h>

#include "Engine/Entity/Entity.h"
#include "Engine/Array.h"
#include "Engine/Manager.h"
#include "CameraSettings.h"
#include "CameraTiming.h"

namespace GL
{
	struct Camera;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#define CAMERA_GET(TYPE) \
    GET(TYPE*, DepthAttachment, (TYPE*) DepthTexture.Get()); \
    GET(TYPE*, DepthAttachmentCopy, (TYPE*) DepthTexture.Get()); \
    NODISCARD ALWAYS_INLINE TYPE* GetAttachment(u8 i) const { return (TYPE*) Attachments[i].Get(); } \
    NODISCARD ALWAYS_INLINE TYPE* GetAttachmentCopy(u8 i) const { return (TYPE*) AttachmentCopies[i].Get(); } \
    template<u8 I, bool COPY> \
    NODISCARD ALWAYS_INLINE TYPE* GetAttachment() const \
    { \
        static_assert(I < GE_MAX_ATTACHMENTS); \
        if constexpr(COPY) return (TYPE*) AttachmentCopies[I].Get(); \
        else return (TYPE*) Attachments[I].Get(); \
    }

namespace gE
{
	class Camera : public Entity
	{
	 public:
		Camera(Window* w, const SizelessCameraSettings&, Entity* = nullptr, Manager* m = nullptr);

		void OnUpdate(float delta) override { }
		void OnRender(float delta) override;

		virtual void GetGLCamera(GL::Camera&);

		GET_CONST(CameraTiming, Timing, _settings.Timing);
		GET_CONST(gE::RenderPass, RenderPass, _settings.RenderPass);
		GET_CONST(gE::ClipPlanes, ClipPlanes, _settings.ClipPlanes);
		GET_CONST(const GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST(const SizelessCameraSettings&, Settings, _settings);

		CAMERA_GET(GL::Texture);

		template<class TEX_T, class CAM_T>
		static void CreateAttachments(CAM_T& cam, const gE::AttachmentSettings& settings);

	 protected:
		virtual void UpdateProjection() = 0;

		GL::FrameBuffer FrameBuffer;
		glm::mat4 Projection;

		gE::SmartPointer<GL::Texture> DepthTexture;
		gE::SmartPointer<GL::Texture> DepthCopy;
		gE::SmartPointer<GL::Texture> Attachments[GE_MAX_ATTACHMENTS]{};
		gE::SmartPointer<GL::Texture> AttachmentCopies[GE_MAX_ATTACHMENTS]{};

	 private:
		bool _isProjectionInvalid = true;
		SizelessCameraSettings _settings;
	};

	class Camera2D : public Camera
	{
	 public:
		Camera2D(Window*, const CameraSettings2D&, Entity* = nullptr, Manager* = nullptr);

		CAMERA_GET(GL::Texture2D);
		GET_CONST(GL::TextureSize2D, Size, _size);
		GET_CONST(float, Aspect, (float) _size.x / _size.y);

		void GetGLCamera(GL::Camera& camera) override;

	 private:
		const GL::TextureSize2D _size;
	};

	class PerspectiveCamera : public Camera2D
	{
	 public:
		PerspectiveCamera(Window*, const PerspectiveCameraSettings&, Entity* = nullptr, Manager* = nullptr);

		template<AngleType T = AngleType::Degree>
		NODISCARD ALWAYS_INLINE float GetFOV() const
		{

			if constexpr(T == AngleType::Radian)
				return _fov;
			return degree_cast<AngleType::Degree>(_fov);
		}

		template<AngleType T = AngleType::Degree>
		ALWAYS_INLINE void SetFOV(float fov)
		{

			if constexpr(T == AngleType::Radian)
				_fov = fov;
			else
				_fov = degree_cast<AngleType::Radian>(fov);
		}

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};

	class OrthographicCamera : public Camera2D
	{
	 public:
		OrthographicCamera(Window*, const OrthographicCameraSettings&, Entity* = nullptr, Manager* = nullptr);

		GET_CONST(const glm::vec4&, Scale, _orthographicScale);

	 protected:
		void UpdateProjection() override;

	 private:
		glm::vec4 _orthographicScale;
	};

	class Camera3D : public Camera
	{
	 public:
		Camera3D(Window*, const CameraSettings3D&, Entity* = nullptr, Manager* = nullptr);

		CAMERA_GET(GL::Texture3D);

		void GetGLCamera(GL::Camera&) override;

		GET_CONST(GL::TextureSize3D, Size, _size);

	 protected:
		void UpdateProjection() override;

	 private:
		const GL::TextureSize3D _size;
	};

	class CameraCubemap : public Camera
	{
	 public:
		CameraCubemap(Window*, const CameraSettings1D&, Entity* = nullptr, Manager* = nullptr);

		CAMERA_GET(GL::TextureCube);

		GET_CONST(GL::TextureSize1D, Size, _size);

		void GetGLCamera(GL::Camera& camera) override;

	 protected:
		void UpdateProjection() override;

	 public:
		const GL::TextureSize1D _size;
	};

	class CameraManager : public ComponentManager<Camera>
	{
	 public:
		using ComponentManager<Camera>::ComponentManager;

		Camera2D* CurrentCamera = nullptr;
		Camera* CallingCamera = nullptr;
	};
}

#pragma clang diagnostic pop