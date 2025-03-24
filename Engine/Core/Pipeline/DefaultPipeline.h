//
// Created by scion on 9/6/2023.
//

#pragma once

#include <Component/Camera/Camera.h>
#include <Component/Camera/PostProcessEffect.h>
#include <Component/Camera/RenderTarget.h>
#include <Graphics/API/GL/Timer.h>
#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Texture/Texture.h>
#include <Graphics/Texture/TextureSettings.h>

namespace gE::DefaultPipeline
{
	GLOBAL GPU::Texture DepthFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_DEPTH_COMPONENT32F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Filter = GPU::FilterMode::Nearest;
		return tex;
	}();

	GLOBAL GPU::Texture HiZFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_R32F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Filter = GPU::FilterMode::Nearest;
		tex.MipCount = 0;
		return tex;
	}();

	GLOBAL GPU::Texture PreviousDepthFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_R32F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Filter = GPU::FilterMode::Nearest;
		return tex;
	}();

	GLOBAL GPU::Texture ColorFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGBA16F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.MipCount = 0;
		return tex;
	}();

	GLOBAL GPU::Texture VelocityFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGB32F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		return tex;
	}();

 	class Target2D final : public RenderTarget<Camera2D>, public DepthTarget<Camera2D>, public ColorTarget<Camera2D>
	{
	 public:
		using TEX_T = API::Texture2D;
		using POSTPROCESS_T = IPostProcessEffect<RenderTarget>;
 		using RenderTarget::GetCamera;

		explicit Target2D(Entity&, Camera2D& camera, const std::vector<POSTPROCESS_T*>&);

		GET(API::Texture2D&, Depth, _depth.Get());
		GET(API::Texture2D&, Color, _color.Get());
		GET(API::Texture2D&, Velocity, _velocity.Get());
		GET(API::Texture2D&, PreviousDepth, _previousDepth);

 		void GetGPUCameraOverrides(GPU::Camera&) const override;

		void RenderDependencies(float) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float) override;
		void Resize() override;

		~Target2D() override = default;

	 private:
 		Attachment<API::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT0> _color;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT1> _velocity;

		API::Texture2D _linearDepth;
 		API::Texture2D _previousColor;
		API::Texture2D _postProcessBack;
		API::Texture2D _previousDepth;

		std::vector<POSTPROCESS_T*> _effects;
	};

	struct Buffers
	{
		explicit Buffers(Window*);

		GET_CONST(const API::Buffer<GPU::Camera>&, Camera, _cameraBuffer);
		GET_CONST(const API::Buffer<GPU::Scene>&, Scene, _sceneBuffer);
		GET_CONST(const API::Buffer<GPU::Lighting>&, Lights, _lightBuffer);

	private:
		API::Buffer<GPU::Camera> _cameraBuffer;
		API::Buffer<GPU::Scene> _sceneBuffer;
		API::Buffer<GPU::Lighting> _lightBuffer;
	};
}

