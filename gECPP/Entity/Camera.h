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

	class Camera : public Entity
	{
	 public:
		Camera(Window* w, const GL::TextureSize&, const ClipPlanes&, RenderPass, const Array<PostProcessPass>*);

		NODISCARD ALWAYS_INLINE GL::TextureSize GetSize() const { return _texture->GetSize(); }
		NODISCARD ALWAYS_INLINE float GetAspect() const { gl::TextureSize size = GetSize(); return (float) size.x / (float) size.y; }

		void OnRender(float delta) override;

		GET_SET(RenderPass, RenderPass, _renderPass);
		GET(const Array<PostProcessPass>*, PostProcessPasses, _postProcessPass);

		virtual GL::Camera GetGLCamera() const = 0;

	 protected:
		virtual void UpdateProjection() = 0;

		ClipPlanes _clipPlanes;

		GL::Texture* _texture{};

		RenderPass _renderPass;
		Array<PostProcessPass>* _postProcessPass;

		gl::mat4 _projection;

		virtual ~Camera() { delete _postProcessPass; }
	};

	class PerspectiveCamera : Camera
	{
	 public:
		PerspectiveCamera(Window* w, float fov, const GL::TextureSize&, const gE::ClipPlanes&, RenderPass, const Array<PostProcessPass>*);

		GET_SET(float, FOV, _fov);

		GL::Camera GetGLCamera() const override;

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};
}
