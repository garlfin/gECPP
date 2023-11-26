//
// Created by scion on 9/6/2023.
//

#pragma once

#include <GL/Buffer/Buffer.h>
#include <GL/Texture/TextureSettings.h>
#include <GL/Texture/Texture.h>
#include <Engine/Component/CameraSettings.h>

#define GE_MAX_INSTANCE 64
#define GE_MAX_LIGHT 4
#define GE_MAX_CUBEMAP 4
#define GL_ALIGN alignas(16)

namespace GL
{
	struct Camera
	{
		glm::vec3 Position;
		u32 Frame;
		GL_ALIGN glm::vec2 ClipPlanes;
		glm::vec2 Parameters;

		TextureHandle ColorTexture;
		TextureHandle DepthTexture;

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
		TextureHandle Depth;
	};

	struct CubemapData
	{
		glm::vec3 Position;
		float BlendRadius;
		glm::vec3 Scale;
		GL_ALIGN TextureHandle Color;
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

		TextureHandle Skybox;

		GL_ALIGN LightData Lights[GE_MAX_LIGHT];
		GL_ALIGN CubemapData Cubemaps[GE_MAX_CUBEMAP];
	};
}

namespace gE::DefaultPipeline
{
	void RenderPass2D(Window*, Camera2D*);
	void RenderPass3D(Window*, Camera3D*);
	void RenderPassShadow(Window* window, Camera2D* camera);
	void RenderPassCubemap(Window*, CameraCubemap*);

	CONST_GLOBAL gE::AttachmentSettings AttachmentColor
	{
		{ GL_NONE }, // Depth Format
		{{ GL_RGBA16F }} // Attachments
	};

	CONST_GLOBAL gE::AttachmentSettings AttachmentDepth
	{
		{ GL_DEPTH_COMPONENT32F }
	};

	CONST_GLOBAL gE::AttachmentSettings AttachmentShadow
	{
		{ GL_DEPTH_COMPONENT16 }
	};

	CONST_GLOBAL gE::AttachmentSettings AttachmentTAA
	{
		{},
		{{}, { GL_RGB16F }} // Velocity
	};

	CONST_GLOBAL gE::AttachmentSettings AttachmentCubemap
	{
		{ GL_DEPTH_COMPONENT16 },
		{{ GL_RGB16F }}
	};

	GLOBAL gE::AttachmentSettings AttachmentsDefault = AttachmentColor | AttachmentDepth;

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

