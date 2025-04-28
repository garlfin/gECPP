#pragma once

#include <Component/Camera/Camera.h>
#include <Component/Camera/PostProcessEffect.h>
#include <Component/Camera/RenderTarget.h>
#include <Graphics/API/GL/Timer.h>
#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Texture/Texture.h>

namespace gE::VRPipeline
{
	class Target2D final : public RenderTarget<Camera2D>, public DepthTarget<Camera2D>, public ColorTarget<Camera2D>
	{
	public:
		using TEX_T = API::Texture2D;
		using POSTPROCESS_T = IPostProcessEffect<RenderTarget>;
 		using RenderTarget::GetCamera;

		explicit Target2D(Entity&, Camera2D& camera, const std::vector<POSTPROCESS_T*>&);

		GET(API::Texture2D&, Depth, _depth.Get());
		GET(API::Texture2D&, Color, _color.Get());

 		void GetGPUCameraOverrides(GPU::Camera&) const override;

		void RenderDependencies(float) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float) override;
		void Resize() override;

		~Target2D() override = default;

	private:
 		Attachment<API::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT0> _color;

 		API::Texture2D _previousColor;
		API::Texture2D _postProcessBack;

		std::vector<POSTPROCESS_T*> _effects;
	};
}

