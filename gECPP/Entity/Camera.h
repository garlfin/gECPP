//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Entity.h"
#include "DefaultPipeline.h"
#include "Array.h"
#include <GL/Texture/Texture.h>

namespace GL { struct Camera; }

namespace gE
{
	class Camera;

	typedef void(*RenderPass)(Camera*);
	typedef void(*PostProcessPass)(Camera*, GL::Texture* in, GL::Texture* out);
	typedef gl::vec2 ClipPlanes;

	struct CameraSettings
	{
		GL::TextureSize Size = {0, 0};
		ClipPlanes ClipPlanes = {0.1, 100};
		RenderPass RenderPass = nullptr;
		Array<PostProcessPass>* PostProcess = nullptr;
	};

	class Camera : public Component
	{
	 public:
		Camera(Entity* w, const CameraSettings&);

		NODISCARD ALWAYS_INLINE GL::TextureSize GetSize() const { return _size; }
		NODISCARD ALWAYS_INLINE float GetAspect() const { gl::TextureSize size = GetSize(); return (float) size.x / (float) size.y; }

		void OnUpdate(float delta) override {}
		void OnRender(float delta) override;

		GET_SET(RenderPass, RenderPass, _renderPass);
		GET(const Array<PostProcessPass>&, PostProcessPasses, _postProcessPass);

		virtual GL::Camera GetGLCamera() const = 0;

	 protected:
		virtual void UpdateProjection() = 0;

		ClipPlanes _clipPlanes;
		GL::TextureSize _size;

		GL::Texture* _texture{};

		RenderPass _renderPass;
		Array<PostProcessPass> _postProcessPass;

		gl::mat4 _projection;
		gl::mat4 _view;
	};

	class PerspectiveCamera : public Camera
	{
	 public:
		PerspectiveCamera(Entity* w, const CameraSettings&, float fov);

		GET_SET(float, FOV, _fov);

		GL::Camera GetGLCamera() const override;

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};
}
