//
// Created by scion on 9/21/2023.
//

#pragma once

#include <GL/Math.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include "Engine/Array.h"
#include "CameraTiming.h"

namespace gE
{
	typedef glm::vec2 ClipPlanes;

	struct SizelessCameraSettings
	{
		RenderPass RenderPass;
		ClipPlanes ClipPlanes = {0.1, 1000};
		CameraTiming Timing;
		const AttachmentSettings& RenderAttachments = DefaultPipeline::AttachmentDefault;
	};

	template<GL::TextureDimension DIMENSION>
	struct CameraSettings : public SizelessCameraSettings
	{
		CameraSettings(const SizelessCameraSettings& settings, const GL::TextureSize<DIMENSION>& size) :
			SizelessCameraSettings(settings), Size(size)
		{};

		GL::TextureSize<DIMENSION> Size { 0 };
	};

	typedef CameraSettings<GL::TextureDimension::D1D> CameraSettings1D;
	typedef CameraSettings<GL::TextureDimension::D2D> CameraSettings2D;
	typedef CameraSettings<GL::TextureDimension::D3D> CameraSettings3D;

	struct PerspectiveCameraSettings : public CameraSettings2D
	{
		float FOV = 80.f;
		AngleType Type = AngleType::Radian;
	};

	struct OrthographicCameraSettings : public CameraSettings2D
	{
		glm::vec4 Scale;
	};
}