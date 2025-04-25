//
// Created by scion on 10/23/2023.
//

#pragma once

#include <Component/Camera/RenderTarget.h>
#include <Core/Pipeline/DefaultPipeline.h>
#include <Entity/Entity.h>

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

	class Light : public Entity, public Managed<Light>
	{
		REFLECTABLE_ONGUI_PROTO(Entity);

	public:
		Light(Window*, Camera&, IDepthTarget&);

		virtual void GetGPULight(GPU::Light& light);

		GET(Camera&, Camera, _camera);
		GET(GL::Texture&, Depth, *Depth);
		GET_SET(vec3, Color, _color);

	protected:
		// Temporary workaround to bizzare bug.
		API::Texture* Depth = DEFAULT;

	private:
		RelativePointer<Camera> _camera;
		vec3 _color = vec3(1.0);
	};

	class LightManager : public Manager<Managed<Light>>
	{
	 public:
		explicit LightManager(Window* window) : _window(window) {};

		DirectionalLight* Sun = DEFAULT;

		void OnRender(float delta, Camera*);

		void UseNearestLights(const vec3& point) const;

	 private:
		Window* _window = DEFAULT;
	};
}
