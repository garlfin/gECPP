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
		Camera(Entity* e, const CameraSettings&);

		void OnUpdate(float delta) override {}
		void OnRender(float delta) override;

		NODISCARD virtual GL::Camera GetGLCamera() const = 0;

		GET_CONST(RenderTarget&, RenderTarget, _renderTarget);
		GET_CONST(Array<PostProcessPass>&, PostProcessPasses, _postProcessPass);
		GET_CONST(GL::TextureSize2D&, Size, _size);
		GET_CONST(float, Aspect, (float) _size.x / _size.y);

		NODISCARD ALWAYS_INLINE GL::Texture* GetAttachment(u8 i) const { return _attachments[i]; }

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		const ClipPlanes _clipPlanes;
		const GL::TextureSize2D _size;

		const gE::RenderTarget& _renderTarget;
		Array<PostProcessPass> _postProcessPass;

		GL::FrameBuffer _frameBuffer;
		GL::Texture2D _depthTexture;
		GL::Texture2D* _attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};

		glm::mat4 _projection;
		glm::mat4 _view;
	};

	class PerspectiveCamera : public Camera
	{
	 public:
		PerspectiveCamera(Entity* e, const PerspectiveCameraSettings& s) :
			Camera(e, s), _fov(s.FOV)
		{

		}

		GET_SET(float, FOV, _fov);

		[[nodiscard]] GL::Camera GetGLCamera() const override;

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};
}
