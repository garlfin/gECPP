//
// Created by scion on 9/6/2023.
//

#pragma once

#include <GL/Buffer/Buffer.h>
#include <GL/Texture/TextureSettings.h>
#include "GL/Texture/Texture.h"

#define GE_MAX_INSTANCE 64
#define GE_MAX_LIGHT 4
#define GE_MAX_CUBEMAP 4
#define GE_MAX_ATTACHMENTS 2
#define GL_ALIGN alignas(16)

namespace GL
{
	struct Camera
	{
		glm::vec3 Position;
		GL_ALIGN glm::vec2 ClipPlanes;
		float FOV;
		u32 State;
		glm::vec4 FutureData; // TODO Find use for space.

		GL_ALIGN glm::mat4 Projection;
		glm::mat4 PreviousViewProjection;
		glm::mat4 View[6];
	};

	enum class LightType : uint
	{
		None,
		Directional,
		Point,
		Spot,
		Area
	};

	struct Light
	{
		glm::mat4 ViewProjection;
		glm::vec3 Color;
		LightType Type;
		glm::vec2 Settings;
		TextureHandle Depth;
	};

	struct Cubemap
	{
		glm::vec3 Position;
		float BlendRadius;
		glm::vec3 Scale;
		GL_ALIGN TextureHandle Color;
	};

	struct Scene
	{
		Light Lights[GE_MAX_LIGHT];
		GL_ALIGN Cubemap Cubemaps[GE_MAX_CUBEMAP];
		uint InstanceCount;
		GL_ALIGN glm::mat4 Model[GE_MAX_INSTANCE];
		glm::mat3x4 Normal[GE_MAX_INSTANCE]; // for alignment purposes.
	};
}


namespace gE
{
	class Camera;
	class Camera2D;
	class Camera3D;
	class CameraCubemap;

	using RenderPass = void(*)(Window*, Camera*);
	using PostProcessFunc = void(*)(Window*, Camera*, GL::Texture* out);

	template<class T>
	using RenderPassT = void(*)(Window*, T*);

	template<class T>
	using PostProcessFuncT = void(*)(Window*, T*, GL::Texture* out);

	struct AttachmentSettings
	{
		GL::SizelessTextureSettings Depth {};
		GL::SizelessTextureSettings Attachments[GE_MAX_ATTACHMENTS] {};
		bool DepthCopy = false;
		bool ColorCopy[GE_MAX_ATTACHMENTS] {};

		constexpr AttachmentSettings& operator|=(const AttachmentSettings& o)
		{
			if(!Depth && o.Depth) Depth = o.Depth;

			for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
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
	void RenderPassCubemap(Window*, CameraCubemap*);

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

	GLOBAL gE::AttachmentSettings AttachmentCubemap
	{
		{ GL_DEPTH_COMPONENT16 },
		{ { GL_RGB16F } }
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

