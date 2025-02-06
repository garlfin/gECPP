//
// Created by scion on 9/6/2023.
//

#pragma once

#include <Component/Camera/Camera.h>
#include <Component/Camera/PostProcessEffect.h>
#include <Component/Camera/RenderTarget.h>
#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Texture/Texture.h>
#include <Graphics/Texture/TextureSettings.h>

#include <Graphics/API/GL/Timer.h>

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

 	class Target2D final : public RenderTarget<Camera2D>, public IDepthTarget, public IColorTarget
	{
	 public:
		using TEX_T = API::Texture2D;
		using POSTPROCESS_T = IPostProcessEffect<RenderTarget>;

		explicit Target2D(Entity&, Camera2D& camera, const std::vector<POSTPROCESS_T*>&);

		GET(API::Texture2D&, Depth, _depth.Get());
		GET(API::Texture2D&, Color, _color.Get());
		GET(API::Texture2D&, Velocity, _velocity.Get());
		GET(API::Texture2D&, PreviousDepth, _previousDepth);

 		void GetGPUCameraOverrides(GPU::Camera&) const override;

		void RenderDependencies(float) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float) override;

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

		ALWAYS_INLINE void UpdateCamera(u64 size = sizeof(GPU::Camera), u64 offset = 0) const
		{
			_cameraBuffer.ReplaceDataDirect((u8*) &Camera + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GPU::Scene), u64 offset = 0) const
		{
			_sceneBuffer.ReplaceDataDirect((u8*) &Scene + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateLighting(u64 size = sizeof(GPU::Lighting), u64 offset = 0) const
		{
			_lightBuffer.ReplaceDataDirect((u8*) &Lighting + offset, size, offset);
		}

		GPU::Camera Camera = DEFAULT;
		GPU::Scene Scene = DEFAULT;
		GPU::Lighting Lighting = DEFAULT;

	 private:
		API::Buffer<GPU::Camera> _cameraBuffer;
		API::Buffer<GPU::Scene> _sceneBuffer;
		API::Buffer<GPU::Lighting> _lightBuffer;
	};
}

