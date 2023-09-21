//
// Created by scion on 9/6/2023.
//

#pragma once

#include <GL/Buffer/Buffer.h>
#include <GL/Texture/TextureSettings.h>
#include "GL/Texture/Texture.h"

#define INSTANCE_MAX_OBJECT 64
#define FRAMEBUFFER_MAX_COLOR_ATTACHMENTS 2

#define GL_ALIGN alignas(16)

namespace GL
{
	struct Camera
	{
		glm::vec3 Position;
		GL_ALIGN glm::vec2 ClipPlanes;
		float FOV;

		GL_ALIGN glm::mat4 Projection;
		glm::mat4 PreviousViewProjection;
		glm::mat4 View[6];
	};

	struct Scene
	{
		uint InstanceCount;
		GL_ALIGN glm::mat4 Model[INSTANCE_MAX_OBJECT];
		glm::mat3x4 Normal[INSTANCE_MAX_OBJECT]; // for alignment purposes.
	};
}

namespace gE
{
	class Camera;

	typedef void(*RenderPass)(Window*, Camera*);
	typedef void(*PostProcessPass)(Window*, Camera*, GL::Texture2D* const (&in)[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS], GL::Texture2D* out);

	struct RenderTarget
	{
		RenderPass RenderPass;
		GL::SizelessTextureSettings Depth;
		GL::SizelessTextureSettings Attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS];
	};
}

namespace gE::DefaultPipeline
{
	void RenderPass2D(Window*, Camera*);
	void RenderPass3D(Window*, Camera*);;
	void RenderPassDirectionalShadow(Window*, Camera*);;

	static const gE::RenderTarget RenderTarget2D
	{
		RenderPass2D,
		{ GL_DEPTH_COMPONENT32F }, // Depth Format
		{ { GL_RGBA16F } }, // Attachments
	};

	static const gE::RenderTarget RenderTarget3D
	{
		RenderPass3D,
		{ GL_DEPTH_COMPONENT32F }, // Depth Format
		{ { GL_RGBA16F } }, // Attachments
	};

	static const gE::RenderTarget RenderTargetDirectionalShadow
	{
		RenderPassDirectionalShadow,
		{ GL_DEPTH_COMPONENT32F }
	};

	struct Buffers
	{
		Buffers(Window*);

		GL::Buffer<GL::Camera> Camera;
		GL::Buffer<GL::Scene> Scene;
	};


}

