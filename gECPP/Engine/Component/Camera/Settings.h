//
// Created by scion on 9/21/2023.
//

#pragma once

#include "GL/Math.h"
#include "Engine/Array.h"
#include "Timing.h"

namespace GL { struct Texture; }

namespace gE
{
	class Camera;
	class Camera2D;
	class Camera3D;
	class CameraCubemap;

	typedef glm::vec2 ClipPlanes;

	struct ICameraSettings
	{
		ClipPlanes ClipPlanes = { 0.1, 1000 };
		CameraTiming Timing = DefaultCameraTiming;
	};

	template<GL::TextureDimension DIMENSION>
	struct CameraSettings : public ICameraSettings
	{
		CameraSettings(const ICameraSettings& settings, const GL::TextureSize<DIMENSION>& size) :
			ICameraSettings(settings), Size(size)
		{ };

		GL::TextureSize<DIMENSION> Size{ 0 };
	};

	typedef CameraSettings<GL::TextureDimension::D1D> CameraSettings1D;
	typedef CameraSettings<GL::TextureDimension::D2D> CameraSettings2D;
	typedef CameraSettings<GL::TextureDimension::D3D> CameraSettings3D;

	struct PerspectiveCameraSettings : public CameraSettings2D
	{
		PerspectiveCameraSettings(const CameraSettings2D& s, float f = 80.f, AngleType t = AngleType::Degree) :
			CameraSettings2D(s),
			FOV(f), Type(t)
		{
		}

		float FOV = 80.f;
		AngleType Type = AngleType::Degree;
	};

	struct OrthographicCameraSettings : public CameraSettings2D
	{
		OrthographicCameraSettings(const CameraSettings2D& s, const glm::vec4& scale) : CameraSettings2D(s), Scale(scale) {};
		glm::vec4 Scale;
	};
}