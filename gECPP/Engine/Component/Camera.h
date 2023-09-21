//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Array.h"
#include "Engine/ComponentManager.h"
#include "GL/Texture/Texture.h"

namespace GL { struct Camera; }

namespace gE
{
	class Camera;

	typedef void(*RenderPass)(Window*, Camera*);
	typedef void(*PostProcessPass)(Window*, Camera*, GL::Texture2D* in, GL::Texture2D* out);

	typedef glm::vec2 ClipPlanes;

	struct CameraSettings
	{
		GL::TextureSize2D Size = {0, 0};
		ClipPlanes ClipPlanes = {0.1, 100};
		RenderPass RenderPass = nullptr;
		Array<PostProcessPass>* PostProcess = nullptr;
	};

	struct PerspectiveCameraSettings : public CameraSettings
	{
		float FOV = degree_cast<AngleType::Radian>(80.f);
	};

	class Camera : public Component
	{
	 public:
		Camera(Entity* e, const CameraSettings&);

		NODISCARD ALWAYS_INLINE GL::TextureSize2D GetSize() const { return _size; }
		NODISCARD ALWAYS_INLINE float GetAspect() const { return (float) _size.x / (float) _size.y; }

		void OnUpdate(float delta) override {}
		void OnRender(float delta) override;

		GET_SET(RenderPass, RenderPass, _renderPass);
		GET_CONST(Array<PostProcessPass>&, PostProcessPasses, _postProcessPass);

		NODISCARD virtual GL::Camera GetGLCamera() const = 0;

		~Camera() override;

	 protected:
		virtual void UpdateProjection() = 0;

		ClipPlanes _clipPlanes;
		GL::TextureSize2D _size;

		RenderPass _renderPass;
		Array<PostProcessPass> _postProcessPass;

		glm::mat4 _projection;
		glm::mat4 _view;
	};

	template<typename T>
	class PerspectiveCamera : public Camera, public T
	{
	 public:
		PerspectiveCamera(Entity* e, const PerspectiveCameraSettings& s, const T::Settings& c, float fov) :
			Camera(e, s), T(s, c), _fov(fov) {}

		GET_SET(float, FOV, _fov);

		[[nodiscard]] GL::Camera GetGLCamera() const override;

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};
}
