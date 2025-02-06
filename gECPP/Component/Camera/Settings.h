//
// Created by scion on 9/21/2023.
//

#pragma once

#include <Math/Math.h>
#include "Timing.h"

namespace GL { class Texture; }

namespace gE
{
	class Camera;
	class Camera2D;
	class Camera3D;
	class CameraCube;

	typedef glm::vec2 ClipPlanes;

	struct ICameraSettings
	{
		ClipPlanes ClipPlanes = { 0.1, 1000 };
		CameraTiming Timing = DefaultCameraTiming;
	};

	template<Dimension DIMENSION>
	struct CameraSettings : public ICameraSettings
	{
		CameraSettings(const ICameraSettings& settings, const Size<DIMENSION>& size) :
			ICameraSettings(settings), Size(size)
		{ };

		Size<DIMENSION> Size{ 0 };
	};

	typedef CameraSettings<Dimension::D1D> CameraSettings1D;
	typedef CameraSettings<Dimension::D2D> CameraSettings2D;
	typedef CameraSettings<Dimension::D3D> CameraSettings3D;

	struct PerspectiveCameraSettings : public CameraSettings2D
	{
		PerspectiveCameraSettings(const CameraSettings2D& s, float f = 80.f, AngleType t = AngleType::Degree) :
			CameraSettings2D(s),
			FOV(f), Type(t)
		{
		}

		float FOV = 80.f; // Y FOV
		AngleType Type = AngleType::Degree;
	};

	struct OrthographicCameraSettings : public CameraSettings2D
	{
		OrthographicCameraSettings(const CameraSettings2D& s, const glm::vec4& scale) : CameraSettings2D(s), Scale(scale) {};
		glm::vec4 Scale;
	};
}