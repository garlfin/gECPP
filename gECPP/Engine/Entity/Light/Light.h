//
// Created by scion on 10/23/2023.
//

#pragma once

#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Entity/Entity.h>
#include <Engine/Renderer/DefaultPipeline.h>

namespace gE
{
	class Light;
	class DirectionalLight;

	GLOBAL GPU::Texture ShadowMapFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_DEPTH_COMPONENT16;
		tex.WrapMode = GPU::WrapMode::Clamp;
		return tex;
	}();

	class LightManager : public Manager<Managed<Light>>
	{
	 public:
		explicit LightManager(Window* window) : _window(window) {};

		DirectionalLight* Sun;

		void OnRender(float delta, Camera*);

		void UseNearestLights(const glm::vec3& point) const;

	 private:
		Window* _window;
	};

	class Light : public Entity, public IDepthTarget, public Managed<Light>
	{
	 public:
		Light(Window*, Camera&, IDepthTarget&);

		virtual void GetGPULight(GPU::Light& light) = 0;

		GET(Camera&, Camera, _camera);

	 private:
		Managed<Light> _managedLight;
		Camera& _camera;
	};
}
