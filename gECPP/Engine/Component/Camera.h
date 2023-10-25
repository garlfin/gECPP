//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Array.h"
#include "Engine/Manager.h"
#include "GL/Texture/Texture.h"
#include "GL/Buffer/FrameBuffer.h"
#include "CameraSettings.h"

namespace GL { struct Camera; }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

#define CAMERA_GET(TYPE) \
	GET(TYPE*, DepthAttachment, (TYPE*) DepthTexture.Get()); \
	GET(TYPE*, DepthAttachmentCopy, (TYPE*) DepthTexture.Get()); \
	NODISCARD ALWAYS_INLINE TYPE* GetAttachment(u8 i) const { return (TYPE*) Attachments[i].Get(); } \
	NODISCARD ALWAYS_INLINE TYPE* GetAttachmentCopy(u8 i) const { return (TYPE*) AttachmentCopies[i].Get(); } \
	template<u8 I, bool COPY> \
	NODISCARD ALWAYS_INLINE TYPE* GetAttachment() const      \
	{                       \
    	if constexpr(COPY) return (TYPE*) *AttachmentCopies[I].Get(); \
		else return (TYPE*) *Attachments[I].Get(); \
	}

namespace gE
{
	class Camera : public Component
	{
	 public:
		Camera(Entity* e, const SizelessCameraSettings&);

		void OnUpdate(float delta) override {}
		void OnRender(float delta) final;

		virtual void GetGLCamera(GL::Camera&) const;

		GET_CONST_VALUE(RenderPass, RenderPass, Settings.RenderPass);
		GET_CONST(GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST_VALUE(ClipPlanes, ClipPlanes, Settings.ClipPlanes);
		GET_CONST(SizelessCameraSettings&, Settings, Settings);

		CAMERA_GET(GL::Texture);

		template<class TEX_T, class CAM_T>
		static void CreateAttachments(CAM_T& cam, const gE::AttachmentSettings& settings);

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		GL::FrameBuffer FrameBuffer;
		glm::mat4 Projection;

		const SizelessCameraSettings Settings;

		gE::Reference<GL::Texture> DepthTexture;
		gE::Reference<GL::Texture> DepthCopy;
		gE::Reference<GL::Texture> Attachments[GE_MAX_ATTACHMENTS] {};
		gE::Reference<GL::Texture> AttachmentCopies[GE_MAX_ATTACHMENTS] {};

	 private:
		bool _invalidated = true;
	};

	class Camera2D : public Camera
	{
	 public:
		Camera2D(Entity*, const CameraSettings2D&);

		CAMERA_GET(GL::Texture2D);

		GET_CONST_VALUE(GL::TextureSize2D, Size, _size);
		GET_CONST_VALUE(float, Aspect, (float) _size.x / _size.y);

	 private:
		const GL::TextureSize2D _size;
	};

	class PerspectiveCamera : public Camera2D
	{
	 public:
		PerspectiveCamera(Entity* e, const PerspectiveCameraSettings& s);

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
		OrthographicCamera(Entity* e, const OrthographicCameraSettings& s);

		GET_CONST(glm::vec4&, Scale, _orthographicScale);

	 protected:
		void UpdateProjection() override;

	 private:
		glm::vec4 _orthographicScale;
	};

	class Camera3D : public Camera
	{
	 public:
		Camera3D(Entity*, const CameraSettings3D&);

		CAMERA_GET(GL::Texture3D);

		void GetGLCamera(GL::Camera&) const override;

		GET_CONST_VALUE(GL::TextureSize3D, Size, _size);

	 protected:
		void UpdateProjection() override;

	 private:
		const GL::TextureSize3D _size;
	};

	class CameraCubemap : public Camera
	{
	 public:
		CameraCubemap(Entity*, const CameraSettings1D&);

		CAMERA_GET(GL::TextureCubemap);

		void GetGLCamera(GL::Camera& camera) const override;

		GET_CONST_VALUE(GL::TextureSize1D, Size, _size);

	 protected:
		void UpdateProjection() override;

	 public:
		const GL::TextureSize1D _size;
	};

	class CameraManager : public ComponentManager<Camera>
	{
	 public:
		using ComponentManager<Camera>::ComponentManager;

		GET_SET_VALUE(Camera*, CurrentCamera, _currentCamera);

	 private:
		Camera* _currentCamera = nullptr;
	};
}

#pragma clang diagnostic pop