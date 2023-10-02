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
		u32 State;

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
	class Camera2D;
	class Camera3D;

	using RenderPass = void(*)(Window*, Camera*);
	using PostProcessFunc = void(*)(Window*, Camera*, GL::Texture* out);

	template<class T>
	using RenderPassT = void(*)(Window*, T*);

	template<class T>
	using PostProcessFuncT = void(*)(Window*, T*, GL::Texture* out);

	struct AttachmentSettings
	{
		GL::SizelessTextureSettings Depth {};
		GL::SizelessTextureSettings Attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};
		bool DepthCopy = false;
		bool ColorCopy[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};

		constexpr AttachmentSettings& operator|=(const AttachmentSettings& o)
		{
			if(!Depth && o.Depth) Depth = o.Depth;

			for(u8 i = 0; i < FRAMEBUFFER_MAX_COLOR_ATTACHMENTS; i++)
				if(!Attachments[i] && o.Attachments[i]) Attachments[i] = o.Attachments[i];

			return *this;
		}

		inline constexpr AttachmentSettings operator|(const AttachmentSettings& o) const { AttachmentSettings settings = *this; return settings |= o; };
	};

	struct PostProcessPass
	{
		PostProcessFunc Func;
		const AttachmentSettings& Requirements;

#ifdef DEBUG
		NODISCARD bool CheckRequirements(const Camera&) const; // i should really decide when i use pointers and references
#endif
	};
}

namespace gE::DefaultPipeline
{
	void RenderPass2D(Window*, Camera2D*);
	void RenderPass3D(Window*, Camera3D*);
	void RenderPassDirectionalShadow(Window*, Camera2D*);

	GLOBAL gE::AttachmentSettings AttachmentColor
	{
		{ GL_NONE }, // Depth Format
		{ { GL_RGBA16F } } // Attachments
	};

	GLOBAL gE::AttachmentSettings AttachmentDepth
	{
		{ GL_DEPTH_COMPONENT32F }
	};

	GLOBAL gE::AttachmentSettings AttachmentShadow
	{
		{ GL_DEPTH_COMPONENT16 }
	};

	GLOBAL gE::AttachmentSettings AttachmentTAA
	{
		{},
		{ {}, { GL_RGB16F } } // Velocity
	};

	GLOBAL gE::AttachmentSettings AttachmentDefault = AttachmentColor | AttachmentDepth;
	GLOBAL gE::AttachmentSettings AttachmentDefaultTAA = AttachmentDefault | AttachmentTAA;

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

		GL::Camera Camera;
		GL::Scene Scene;

	 private:
		GL::Buffer<GL::Camera> _cameraBuffer;
		GL::Buffer<GL::Scene> _sceneBuffer;
	};
}

