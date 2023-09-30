//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Array.h"
#include "Engine/ComponentManager.h"
#include "GL/Texture/Texture.h"
#include "GL/Buffer/FrameBuffer.h"
#include "CameraSettings.h"

namespace GL { struct Camera; }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

namespace gE
{
	class Camera : public Component
	{
	 public:
		Camera(Entity* e, const SizelessCameraSettings&);

		void OnUpdate(float delta) override {}
		void OnRender(float delta) final;

		void GetGLCamera(GL::Camera&) const;

		GET_CONST_VALUE(RenderPass, RenderPass, Settings.RenderPass);
		GET_CONST(GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST_VALUE(ClipPlanes, ClipPlanes, Settings.ClipPlanes);
		GET_CONST(SizelessCameraSettings&, Settings, Settings);

		GET(GL::Texture*, DepthAttachment, DepthTexture);
		GET(GL::Texture*, DepthAttachmentCopy, DepthCopy);
		NODISCARD ALWAYS_INLINE GL::Texture* GetAttachment(u8 i) const { return Attachments[i]; }
		NODISCARD ALWAYS_INLINE GL::Texture* GetAttachmentCopy(u8 i) const { return AttachmentCopies[i]; }

		template<class TEX_T, class CAM_T>
		static void CreateAttachments(CAM_T& cam, const gE::AttachmentSettings& settings);

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		GL::FrameBuffer FrameBuffer;
		glm::mat4 Projection;
		glm::mat4 View;

		const SizelessCameraSettings Settings;

		gE::Reference<GL::Texture> DepthTexture;
		gE::Reference<GL::Texture> DepthCopy;
		gE::Reference<GL::Texture> Attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};
		gE::Reference<GL::Texture> AttachmentCopies[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};

	 private:
		bool _invalidated = true;
	};

	class Camera2D : public Camera
	{
	 public:
		Camera2D(Entity*, const CameraSettings2D&);

		GET(GL::Texture2D*, DepthAttachment, (GL::Texture2D*) DepthTexture.Get());
		GET(GL::Texture2D*, DepthAttachmentCopy, (GL::Texture2D*) DepthTexture.Get());
		NODISCARD ALWAYS_INLINE GL::Texture2D* GetAttachment(u8 i) const { return (GL::Texture2D*) Attachments[i].Get(); }
		NODISCARD ALWAYS_INLINE GL::Texture2D* GetAttachmentCopy(u8 i) const { return (GL::Texture2D*) AttachmentCopies[i].Get(); }

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

		GET(GL::Texture3D*, DepthAttachment, (GL::Texture3D*) DepthTexture.Get());
		GET(GL::Texture3D*, DepthAttachmentCopy, (GL::Texture3D*) DepthTexture.Get());
		NODISCARD ALWAYS_INLINE GL::Texture3D* GetAttachment(u8 i) const { return (GL::Texture3D*) Attachments[i].Get(); }
		NODISCARD ALWAYS_INLINE GL::Texture3D* GetAttachmentCopy(u8 i) const { return (GL::Texture3D*) AttachmentCopies[i].Get(); }

		GET_CONST_VALUE(GL::TextureSize3D, Size, _size);

	 protected:
		void UpdateProjection() override;

	 private:
		const GL::TextureSize3D _size;
	};

	class CubemapCamera : public Camera
	{
	 public:
		CubemapCamera(Entity*, const CameraSettings1D&);

		GET(GL::TextureCubemap*, DepthAttachment, (GL::TextureCubemap*) DepthTexture.Get());
		GET(GL::TextureCubemap*, DepthAttachmentCopy, (GL::TextureCubemap*) DepthTexture.Get());
		NODISCARD ALWAYS_INLINE GL::TextureCubemap* GetAttachment(u8 i) const { return (GL::TextureCubemap*) Attachments[i].Get(); }
		NODISCARD ALWAYS_INLINE GL::TextureCubemap* GetAttachmentCopy(u8 i) const { return (GL::TextureCubemap*) AttachmentCopies[i].Get(); }

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

		GET_SET(Camera*, CurrentCamera, _currentCamera);

	 private:
		Camera* _currentCamera = nullptr;
	};
}

#pragma clang diagnostic pop