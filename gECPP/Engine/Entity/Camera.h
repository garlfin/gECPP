//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Entity.h"
#include "Engine/Renderer/DefaultPipeline.h"
#include "Engine/Array.h"
#include "Engine/ComponentManager.h"
#include "GL/Texture/Texture.h"

namespace GL { struct Camera; }

namespace gE
{
	class Camera;

	typedef void(*RenderPass)(Window*, Camera*);
	typedef void(*PostProcessPass)(Window*, Camera*, GL::Texture<GL::TextureDimension::D2D>* in, GL::Texture<GL::TextureDimension::D2D>* out);
	typedef glm::vec2 ClipPlanes;

	struct CameraSettings
	{
		glm::TextureSize2D Size = {0, 0};
		ClipPlanes ClipPlanes = {0.1, 100};
		RenderPass RenderPass = nullptr;
		Array<PostProcessPass>* PostProcess = nullptr;
	};

	class Camera : public Component
	{
	 public:
		Camera(Entity* w, const CameraSettings&);

		NODISCARD ALWAYS_INLINE glm::TextureSize2D GetSize() const { return _size; }
		NODISCARD ALWAYS_INLINE float GetAspect() const { glm::TextureSize2D size = GetSize(); return (float) size.x / (float) size.y; }

		void OnUpdate(float delta) override {}
		void OnRender(float delta) override;

		GET_SET(RenderPass, RenderPass, _renderPass);
		GET_CONST(Array<PostProcessPass>&, PostProcessPasses, _postProcessPass);

		NODISCARD virtual GL::Camera GetGLCamera() const = 0;

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		ClipPlanes _clipPlanes;
		glm::TextureSize2D _size;

		GL::Texture<GL::TextureDimension::D2D>* _texture;

		RenderPass _renderPass;
		Array<PostProcessPass> _postProcessPass;

		glm::mat4 _projection;
		glm::mat4 _view;
	};

	class PerspectiveCamera : public Camera
	{
	 public:
		PerspectiveCamera(Entity* w, const CameraSettings&, float fov);

		GET_SET(float, FOV, _fov);

		[[nodiscard]] GL::Camera GetGLCamera() const override;

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};
}
