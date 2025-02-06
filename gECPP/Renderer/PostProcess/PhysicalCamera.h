//
// Created by scion on 1/27/2025.
//

#pragma once

#include <Math/Math.h>
#include <Utility/Macro.h>

namespace gE::PostProcess
{
    enum class ExposureMode : u8
	{
		Physical,
		Automatic,
		Manual
	};

    struct PhysicalCameraSettings
    {
        NODISCARD static float EV100(float aperture, float shutter, float ISO);
        NODISCARD static float EV100(float luminance, float middleGray = 12.7f);
        NODISCARD static float EV100ToExposure(float ev100);

        float CalculatePhysicalExposure() const;

        ExposureMode ExposureMode = ExposureMode::Physical;

        // Physical
        float FocalLength = 0.005f;
        float FStop = 0.4f;
        float ShutterTime = 1.f / 24.f;
        float ISO = 300.0f;

        // Automatic
        float TargetExposure = 1.0;

        // Manual
        float Exposure = 0.6f;
    };

	inline float PhysicalCameraSettings::EV100(float aperture, float shutter, float ISO)
	{
		return std::log2(aperture * aperture / shutter * 100.f / ISO);
	}

	inline float PhysicalCameraSettings::EV100(float luminance, float middleGray)
	{
		return std::log2(luminance * 100.f / middleGray);
	}

	inline float PhysicalCameraSettings::EV100ToExposure(float ev100)
	{
		return 1.0 / (1.2f * std::pow(2.0, ev100));
	}

	inline float PhysicalCameraSettings::CalculatePhysicalExposure() const
	{
		return EV100ToExposure(EV100(FStop, ShutterTime, ISO));
	}
};