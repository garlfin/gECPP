//
// Created by scion on 9/6/2023.
//

#pragma once

#include <GL/Buffer/Buffer.h>
#include <GL/Texture/TextureSettings.h>
#include <GL/Texture/Texture.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Component/Camera/Settings.h>

#define GE_MAX_INSTANCE 64
#define GE_MAX_LIGHT 4
#define GE_MAX_CUBEMAP 4

namespace GL
{
	struct Camera
	{
		glm::vec3 Position;
		u32 Frame;
		GL_ALIGN glm::vec2 ClipPlanes;
		glm::vec2 Parameters;

		handle ColorTexture;
		handle DepthTexture;

		GL_ALIGN glm::mat4 PreviousViewProjection;
		glm::mat4 Projection;
		glm::mat4 View[6];
	};

	enum class LightType : u32
	{
		None,
		Directional,
		Point,
		Spot,
		Area
	};

	struct LightData
	{
		glm::mat4 ViewProjection;
		glm::vec3 Position;
		LightType Type;
		glm::vec3 Color;
		u32 PackedSettings;
		glm::vec2 Planes;
		handle Depth;
	};

	struct CubemapData
	{
		glm::vec3 Position;
		float BlendRadius;
		glm::vec3 Scale;
		GL_ALIGN handle Color;
	};

	struct Scene
	{
		uint InstanceCount;
		uint Stage;
		GL_ALIGN glm::mat4 Model[GE_MAX_INSTANCE];
		glm::mat4 PreviousModel[GE_MAX_INSTANCE];
		glm::mat3x4 Normal[GE_MAX_INSTANCE]; // for alignment purposes.
	};

	struct Lighting
	{
		u32 LightCount = 1;
		u32 CubemapCount = 1;

		handle Skybox;

		GL_ALIGN LightData Lights[GE_MAX_LIGHT];
		GL_ALIGN CubemapData Cubemaps[GE_MAX_CUBEMAP];
	};
}

namespace gE::DefaultPipeline
{
	CONSTEXPR_GLOBAL GL::ITextureSettings DepthFormat { GL_DEPTH_COMPONENT32F, GL::WrapMode::Clamp };
	CONSTEXPR_GLOBAL GL::ITextureSettings ColorFormat { GL_RGBA16F, GL::WrapMode::Clamp };
	CONSTEXPR_GLOBAL GL::ITextureSettings VelocityFormat { GL_RG16F, GL::WrapMode::Clamp };

 	class Target2D : public RenderTarget<Camera2D>, public IDepthTarget
	{
	 public:
		explicit Target2D(Camera2D& camera);

		GET(GL::Texture2D&, Depth, _depth.Get());
		GET(GL::Texture2D&, Color, _color.Get());
		GET(GL::Texture2D&, Velocity, _velocity.Get());

		void RenderPass() override;

	 private:
		Attachment<GL::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<GL::Texture2D, GL_COLOR_ATTACHMENT0> _color;
		Attachment<GL::Texture2D, GL_COLOR_ATTACHMENT1> _velocity;
	};

	struct Buffers
	{
		explicit Buffers(Window*);

		ALWAYS_INLINE void UpdateCamera(u64 size = sizeof(GL::Camera), u64 offset = 0) const
		{
			_cameraBuffer.ReplaceData((u8*) &Camera + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GL::Scene), u64 offset = 0) const
		{
			_sceneBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		ALWAYS_INLINE void UpdateLighting(u64 size = sizeof(GL::Lighting), u64 offset = 0) const
		{
			_lightBuffer.ReplaceData((u8*) &Lighting + offset, size, offset);
		}

		GL::Camera Camera;
		GL::Scene Scene;
		GL::Lighting Lighting;

	 private:
		GL::Buffer<GL::Camera> _cameraBuffer;
		GL::Buffer<GL::Scene> _sceneBuffer;
		GL::Buffer<GL::Lighting> _lightBuffer;
	};
}

