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

	class Light : public Entity, public Managed<Light>
	{
	 public:
		Light(Window*, Camera&, IDepthTarget&);

		virtual void GetGPULight(GPU::Light& light);

		GET(Camera&, Camera, _camera);
		GET(GL::Texture&, Depth, *Depth);
		GET_SET_VALUE(glm::vec3, Color, _color);

	 protected:
		// Temporary workaround to bizzare bug.
		API::Texture* Depth;

	 private:
		RelativePointer<Camera> _camera;
		glm::vec3 _color = glm::vec3(1.0);
	};
}
