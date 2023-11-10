//
// Created by scion on 9/21/2023.
//

#pragma once

#include <GL/Math.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include "Engine/Array.h"
#include "CameraTiming.h"

#define GE_MAX_ATTACHMENTS 2

namespace gE
{
	class Camera;
	class Camera2D;
	class Camera3D;
	class CameraCubemap;

	typedef glm::vec2 ClipPlanes;

	using RenderPass = void (*)(Window*, Camera*);
	using PostProcessFunc = void (*)(Window*, Camera*, GL::Texture* out);

	template<class T>
	using RenderPassT = void (*)(Window*, T*);

	template<class T>
	using PostProcessFuncT = void (*)(Window*, T*, GL::Texture* out);

	struct AttachmentSettings
	{
		GL::SizelessTextureSettings Depth{};
		GL::SizelessTextureSettings Attachments[GE_MAX_ATTACHMENTS]{};
		bool CopyDepth = false;
		bool CopyAttachment[GE_MAX_ATTACHMENTS]{};

		constexpr AttachmentSettings& operator|=(const AttachmentSettings& o)
		{
			if(!Depth && o.Depth) Depth = o.Depth;
			else if(Depth && o.Depth) LOG("WARNING: Conflicting depth attachment!");

			CopyDepth |= o.CopyDepth;

			for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
			{
				if (!Attachments[i] && o.Attachments[i]) Attachments[i] = o.Attachments[i];
				else if (Attachments[i] && o.Attachments[i]) LOG("WARNING: Conflicting color attachment!");

				CopyAttachment[i] |= o.CopyAttachment[i];
			}

			return *this;
		}

		inline constexpr AttachmentSettings operator|(const AttachmentSettings& o) const
		{
			AttachmentSettings settings = *this;
			return settings |= o;
		};
	};

	struct PostProcessPass
	{
		PostProcessFunc Func;
		const AttachmentSettings& Requirements;

#ifdef DEBUG
		NODISCARD bool CheckRequirements(const Camera&) const;
#endif
	};

	struct SizelessCameraSettings
	{
		RenderPass RenderPass;
		ClipPlanes ClipPlanes = { 0.1, 1000 };
		CameraTiming Timing = DefaultCameraTiming;
		AttachmentSettings RenderAttachments;
	};

	template<GL::TextureDimension DIMENSION>
	struct CameraSettings : public SizelessCameraSettings
	{
		CameraSettings(const SizelessCameraSettings& settings, const GL::TextureSize<DIMENSION>& size) :
			SizelessCameraSettings(settings), Size(size)
		{ };

		GL::TextureSize<DIMENSION> Size{ 0 };
	};

	typedef CameraSettings<GL::TextureDimension::D1D> CameraSettings1D;
	typedef CameraSettings<GL::TextureDimension::D2D> CameraSettings2D;
	typedef CameraSettings<GL::TextureDimension::D3D> CameraSettings3D;

	struct PerspectiveCameraSettings : public CameraSettings2D
	{
		PerspectiveCameraSettings(const CameraSettings2D& s, float f = 80.f, AngleType t = AngleType::Degree) : CameraSettings2D(s),
			FOV(f), Type(t)
		{}

		float FOV = 80.f;
		AngleType Type = AngleType::Degree;
	};

	struct OrthographicCameraSettings : public CameraSettings2D
	{
		OrthographicCameraSettings(const CameraSettings2D& s, const glm::vec4& scale) : CameraSettings2D(s), Scale(scale) {};
		glm::vec4 Scale;
	};
}