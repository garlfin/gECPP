//
// Created by scion on 9/29/2023.
//

#include "VRPipeline.h"

#include <utility>
#include <Window.h>
#include <Entity/Light/DirectionalLight.h>
#include <Entity/Light/Light.h>
#include <glm/gtx/string_cast.hpp>

#define HIZ_MODE_COPY 0
#define HIZ_MODE_DOWNSAMPLE 1

#define HIZ_GROUP_SIZE 8

namespace gE::VRPipeline
{
	void Target2D::RenderPass(float delta, Camera*)
	{
		Camera2D& camera = RenderTarget::GetCamera();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.RenderState = RenderState::PreZ;
		window.RenderState.UsePostProcess = false;
		camera.GetFlagOverrides(window.RenderState);

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		camera.SetViewport();

		window.GetRenderers().OnRender(0.f, &camera);

		window.GetLights().UseNearestLights(vec3(0.0f));
		window.GetCubemaps().UseNearestCubemaps(vec3(0.0f));

		// COLOR
		window.RenderState = RenderState::PreZForward;
		window.RenderState.UsePostProcess = false;
		camera.GetFlagOverrides(window.RenderState);

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		glLineWidth(2.f);

#ifdef DEBUG
		window.GetCubemaps().DrawSkybox(); // Allow wireframes to draw on top
		window.GetRenderers().OnRender(0.f, &camera);
#else
		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
#endif
	}

	void Target2D::PostProcessPass(float)
	{
		// Post process loop
		API::Texture2D* front = &*_color, *back = &_postProcessBack;
		for(POSTPROCESS_T* effect : _effects)
			if(effect->RenderPass(*front, *back))
				std::swap(front, back);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Sync post process "backbuffer" and main color buffer
		if(front == &_postProcessBack) _color->CopyFrom(_postProcessBack);
	}

	Target2D::Target2D(Entity& owner, Camera2D& camera, const std::vector<POSTPROCESS_T*>& effects) :
		RenderTarget(owner, camera),
		DepthTarget(camera, *_depth),
		ColorTarget(camera, *_color),
		_effects(effects)
	{
		Target2D::Resize();
	}

	void Target2D::Resize()
	{
		if(_depth->GetSize() == GetSize()) return;

		PlacementNew(_depth, GetFrameBuffer(), GPU::Texture2D(DefaultPipeline::DepthFormat, GetSize()));
		PlacementNew(_color, GetFrameBuffer(), GPU::Texture2D(DefaultPipeline::ColorFormat, GetSize()));
		PlacementNew(_postProcessBack, &GetWindow(), GPU::Texture2D(DefaultPipeline::ColorFormat, GetSize()));
	}

	void Target2D::GetGPUCameraOverrides(GPU::Camera& camera) const
	{
		//camera.ColorTexture = (handle) _previousColor;
	}

	void Target2D::RenderDependencies(float delta)
	{
		Camera3D* reflectionSystem = GetWindow().GetReflectionSystem();
		LightManager& lightManager = GetWindow().GetLights();

		if(reflectionSystem) reflectionSystem->OnRender(delta, &RenderTarget::GetCamera());
		if(lightManager.Sun) lightManager.Sun->GetCamera().OnRender(delta, &RenderTarget::GetCamera());
	}
}