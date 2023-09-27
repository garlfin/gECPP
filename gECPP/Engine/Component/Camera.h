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

namespace gE
{
	class Camera : public Component
	{
	 public:
		Camera(Entity* e, const SizelessCameraSettings&);

		void OnUpdate(float delta) override {}
		void OnRender(float delta) override;

		void GetGLCamera(GL::Camera&) const;

		GET_CONST(RenderPass, RenderPass, Settings.RenderPass);
		GET_CONST(GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST(ClipPlanes, ClipPlanes, Settings.ClipPlanes);
		GET(GL::Texture*, DepthAttachment, DepthTexture);
		GET(GL::Texture*, DepthAttachmentCopy, DepthCopy);
		GET_CONST(SizelessCameraSettings&, Settings, Settings);

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

		GL::Texture2D* DepthTexture = nullptr;
		GL::Texture2D* DepthCopy = nullptr;
		GL::Texture2D* Attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};
		GL::Texture2D* AttachmentCopies[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};
	};

	class Camera2D : public Camera
	{
	 public:
		Camera2D(Entity*, const CameraSettings2D&);

		GET_CONST_VALUE(GL::TextureSize2D, Size, _size);
		GET_CONST_VALUE(float, Aspect, (float) _size.x / _size.y);

	 private:
		const GL::TextureSize2D _size;
	};

	class PerspectiveCamera : public Camera2D
	{
	 public:
		PerspectiveCamera(Entity* e, const PerspectiveCameraSettings& s);

		GET_SET(float, FOV, _fov);

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
}
