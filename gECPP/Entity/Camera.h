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
		Camera(Window* w, const GL::TextureSize&, float fov, const ClipPlanes&, RenderPass, const Array<PostProcessPass>&);

		NODISCARD ALWAYS_INLINE GL::TextureSize GetSize() const { return _texture->GetSize(); }

		GET_SET(float, FOV, _fov);
		GET_SET(RenderPass, RenderPass, _renderPass);
		GET_SET(const Array<PostProcessPass>&, PostProcessPasses, _postProcessPass);

		virtual void GetGLCamera(const GL::Camera& camera) const = 0;

	 private:

		float _fov;
		ClipPlanes _clipPlanes;

		GL::Texture* _texture;

		RenderPass _renderPass;
		Array<PostProcessPass> _postProcessPass;
	};
}
