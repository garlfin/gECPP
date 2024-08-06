//
// Created by scion on 9/6/2023.
//

#pragma once

#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Texture/TextureSettings.h>
#include <Graphics/Texture/Texture.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Engine/WindowState.h>

#define GE_MAX_INSTANCE 64
#define GE_MAX_LIGHT 4
#define GE_MAX_CUBEMAP 4

namespace GPU
{
	struct Camera
	{
		glm::vec3 Position;
		u32 Frame;
		glm::vec2 ClipPlanes;
		glm::vec2 Size;
		glm::vec4 Parameters;

		handle ColorTexture;
		handle DepthTexture;

		glm::mat4 PreviousViewProjection;
		glm::mat4 Projection;
		glm::mat4 View[6];
	};

	enum class LightType : u32
	{
		Directional,
		Point,
		Spot,
		Area
	};

	struct Light
	{
		glm::mat4 ViewProjection;
		glm::vec3 Position;
		LightType Type;
		glm::vec3 Color;
		u32 PackedSettings;
		glm::vec2 Planes;
		handle Depth;
	};

	enum class CubemapType : u32
	{
		Infinite,
		AABB,
		Sphere,
		None = Infinite
	};

	struct Cubemap
	{
		glm::vec3 Position;
		float BlendRadius;
		glm::vec3 Scale;
		CubemapType Type;
		API_ALIGN handle Color;
	};

	struct Scene
	{
		u32 InstanceCount;
		gE::RenderFlags State;
		API_ALIGN glm::mat4 Model[GE_MAX_INSTANCE];
		glm::mat4 PreviousModel[GE_MAX_INSTANCE];
		glm::mat3x4 Normal[GE_MAX_INSTANCE]; // for alignment purposes.
	};

	struct Lighting
	{
		u32 LightCount = 1;
		u32 CubemapCount = 1;

		handle Skybox;

		API_ALIGN Light Lights[GE_MAX_LIGHT];
		API_ALIGN Cubemap Cubemaps[GE_MAX_CUBEMAP];
	};
}

namespace gE::DefaultPipeline
{
	CONSTEXPR_GLOBAL GPU::ITextureSettings DepthFormat { GL_DEPTH_COMPONENT32, GPU::WrapMode::Clamp, GPU::FilterMode::Nearest, 1 };
	CONSTEXPR_GLOBAL GPU::ITextureSettings HiZFormat { GL_R32F, GPU::WrapMode::Clamp, GPU::FilterMode::Nearest, 0 };
	CONSTEXPR_GLOBAL GPU::ITextureSettings ColorFormat { GL_RGBA16F, GPU::WrapMode::Clamp, GPU::FilterMode::Linear, 0 };
	CONSTEXPR_GLOBAL GPU::ITextureSettings VelocityFormat { GL_RG32F, GPU::WrapMode::Clamp };

 	class Target2D : public RenderTarget<Camera2D>, public IDepthTarget, public IColorTarget
	{
	 public:
		typedef API::Texture2D TEX_T;
		explicit Target2D(Entity&, Camera2D& camera, const std::vector<PostProcessEffect<Target2D>*>&);

		GET(API::Texture2D&, Depth, _depth.Get());
		GET(API::Texture2D&, Color, _color.Get());
		GET(API::Texture2D&, Velocity, _velocity.Get());

		void RenderDependencies(float) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float) override;

	 private:
		Attachment<API::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT0> _color;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT1> _velocity;

		API::Texture2D _colorBack;
		API::Texture2D _depthBack;
		API::Texture2D _postProcessBack;

		std::vector<PostProcessEffect<Target2D>*> _effects;
	};

	struct Buffers
	{
		explicit Buffers(Window*);

		ALWAYS_INLINE void UpdateCamera(u64 size = sizeof(GPU::Camera), u64 offset = 0) const
		{
			_cameraBuffer.ReplaceData((u8*) &Camera + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GPU::Scene), u64 offset = 0) const
		{
			_sceneBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateLighting(u64 size = sizeof(GPU::Lighting), u64 offset = 0) const
		{
			_lightBuffer.ReplaceData((u8*) &Lighting + offset, size, offset);
		}

		GPU::Camera Camera;
		GPU::Scene Scene;
		GPU::Lighting Lighting;

	 private:
		API::Buffer<GPU::Camera> _cameraBuffer;
		API::Buffer<GPU::Scene> _sceneBuffer;
		API::Buffer<GPU::Lighting> _lightBuffer;
	};
}

