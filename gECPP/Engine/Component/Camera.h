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
    GET(TYPE*, Depth, (TYPE*) Depth.Get()); \
    GET(TYPE*, Color, (TYPE*) Attachments[0].Get()); \
    GET(TYPE*, ColorCopy, (TYPE*) Attachments[GE_MAX_ATTACHMENTS].Get()); \
    NODISCARD ALWAYS_INLINE TYPE* GetAttachment(u8 i) const { return (TYPE*) Attachments[i].Get(); } \
    template<u8 I> \
    NODISCARD ALWAYS_INLINE TYPE* GetAttachment() const \
    { \
        return (TYPE*) Attachments[I].Get(); \
    }

namespace gE
{
	class Camera : public Component
	{
	 public:
		Camera(Entity* e, Manager*, const SizelessCameraSettings&);

		void OnUpdate(float delta) override { }
		void OnRender(float delta) final;

		virtual void GetGLCamera(GL::Camera&);

		GET_CONST(CameraTiming, Timing, _settings.Timing);
		GET_CONST(gE::RenderPass, RenderPass, _settings.RenderPass);
		GET_CONST(gE::ClipPlanes, ClipPlanes, _settings.ClipPlanes);
		GET_CONST(const GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST(const SizelessCameraSettings&, Settings, _settings);
		GET_CONST(const AttachmentSettings&, AttachmentSettings, _settings.Attachments);
		GET_CONST(const glm::mat4&, Projection, Projection);

		CAMERA_GET(GL::Texture);

		template<class TEX_T, class CAM_T>
		static void CreateAttachments(CAM_T&);

	 protected:
		virtual void UpdateProjection() = 0;

		GL::FrameBuffer FrameBuffer;
		glm::mat4 Projection;

		gE::SmartPointer<GL::Texture> Depth;
		gE::SmartPointer<GL::Texture> Attachments[GE_MAX_ATTACHMENTS + 1]{};

		u32 Frame = 0;

	 private:
		SizelessCameraSettings _settings;

		bool _isProjectionInvalid = true;
	};

	class Camera2D : public Camera
	{
	 public:
		Camera2D(Entity*, Manager*, const CameraSettings2D&);

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
		PerspectiveCamera(Entity* e, Manager*, const PerspectiveCameraSettings& s);

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
		OrthographicCamera(Entity* e, Manager*, const OrthographicCameraSettings& s);

		GET_CONST(const glm::vec4&, Scale, _orthographicScale);

	 protected:
		void UpdateProjection() override;

	 private:
		glm::vec4 _orthographicScale;
	};

	class Camera3D : public Camera
	{
	 public:
		Camera3D(Entity*, Manager*, const CameraSettings3D&);

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
		CameraCubemap(Entity*, Manager*, const CameraSettings1D&);

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

		Camera* CurrentCamera = nullptr;
		Camera* CallingCamera = nullptr;
	};
}

#pragma clang diagnostic pop