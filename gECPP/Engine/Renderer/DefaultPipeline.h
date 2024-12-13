//
// Created by scion on 9/6/2023.
//

#pragma once

#include <Engine/Window/WindowState.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Texture/Texture.h>
#include <Graphics/Texture/TextureSettings.h>

#define API_MAX_INSTANCE 64
#define API_MAX_LIGHT 4
#define API_MAX_CUBEMAP 4

namespace GPU
{
	struct Camera
	{
		glm::vec3 Position;
		u32 Frame;
		glm::vec2 Planes; // Near, Far
		glm::ivec2 Size; // Viewport Size
		glm::vec3 Parameters; // Up to implementation
		float FrameDelta;

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

	struct ObjectInfo
	{
		glm::mat4 Model;
		glm::mat4 PreviousModel;
		glm::mat3x4 Normal;
	};

	struct Scene
	{
		gE::RenderFlags State;
		API_ALIGN u32 InstanceCount[API_MAX_MULTI_DRAW];
		API_ALIGN ObjectInfo Objects[API_MAX_INSTANCE];
	};

	struct Lighting
	{
		u32 LightCount = 1;
		u32 CubemapCount = 1;

		handle Skybox;

		API_ALIGN Light Lights[API_MAX_LIGHT];
		API_ALIGN Cubemap Cubemaps[API_MAX_CUBEMAP];
	};
}

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

		~Target2D() override = default;

	 private:
		Attachment<API::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT0> _color;
		Attachment<API::Texture2D, GL_COLOR_ATTACHMENT1> _velocity;

		API::Texture2D _taaBack;
		API::Texture2D _depthBack;
		API::Texture2D _postProcessBack;
		API::Texture2D _previousDepth;

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

		GPU::Camera Camera = DEFAULT;
		GPU::Scene Scene = DEFAULT;
		GPU::Lighting Lighting = DEFAULT;

	 private:
		API::Buffer<GPU::Camera> _cameraBuffer;
		API::Buffer<GPU::Scene> _sceneBuffer;
		API::Buffer<GPU::Lighting> _lightBuffer;
	};
}

