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

		virtual void GetGLCamera(GL::Camera&) const = 0;

		GET_CONST(RenderTarget*, RenderTarget, RenderTarget);
		GET_CONST(Array<PostProcessPass>&, PostProcessPasses, PostProcessPasses);
		GET_CONST(GL::FrameBuffer&, FrameBuffer, FrameBuffer);
		GET_CONST(ClipPlanes, ClipPlanes, ClipPlanes);
		GET(GL::Texture*, DepthAttachment, DepthTexture);

		NODISCARD ALWAYS_INLINE GL::Texture* GetAttachment(u8 i) const { return Attachments[i]; }

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		GL::FrameBuffer FrameBuffer;
		glm::mat4 Projection;
		glm::mat4 View;

		const ClipPlanes ClipPlanes;

		const gE::RenderTarget* const RenderTarget;
		Array<PostProcessPass> PostProcessPasses;

		GL::Texture2D* DepthTexture;
		GL::Texture2D* Attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};

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

		void GetGLCamera(GL::Camera&) const override;

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
