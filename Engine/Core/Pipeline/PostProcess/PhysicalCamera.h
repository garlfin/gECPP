//
// Created by scion on 1/27/2025.
//

#pragma once

#include <Core/Macro.h>
#include <Core/Math/Math.h>

namespace gE::PostProcess
{
    enum class ExposureMode : u8
	{
		Physical,
		Automatic,
		Manual
	};

	CONSTEXPR_GLOBAL EnumData<ExposureMode, 3> EExposureMode
	{
		EnumType::Normal,
		{
			REFLECT_ENUM(ExposureMode, Physical),
			REFLECT_ENUM(ExposureMode, Automatic),
			REFLECT_ENUM(ExposureMode, Manual),
		}
	};

    struct PhysicalCameraSettings : public IReflectable
    {
    	REFLECTABLE_ONGUI_PROTO(IReflectable);

    public:
    	PhysicalCameraSettings() = default;

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

    	// LUT
    	Reference<API::Texture3D> LUT;
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

	inline REFLECTABLE_ONGUI_IMPL(PhysicalCameraSettings,
	{
		/*DrawField(EnumField{ "Exposure Mode", "", EExposureMode }, ExposureMode, depth);

		if(ExposureMode == ExposureMode::Manual)
		{
			DrawField(ScalarField{ "Exposure", "", 0.f, 10.f, FLT_EPSILON, ScalarViewMode::Slider }, Exposure, depth);
			return;
		}

		if(ExposureMode == ExposureMode::Physical)
		{
			DrawField(ScalarField{ "F-Stop", "", 0.f, 10.f, FLT_EPSILON, ScalarViewMode::Slider }, FStop, depth);
			float shutter = 1.0 / ShutterTime;
			DrawField(ScalarField{ "Shutter Time", "Frames per Second", 1.f, FLT_MAX, 1.f }, shutter, depth);
			ShutterTime = 1.0 / shutter;
			DrawField(ScalarField{ "ISO", "", 1.f, FLT_MAX, 1.f }, ISO, depth);
		}

		DrawField<const float>(ScalarField<float>{ "Exposure" }, Exposure, depth);

		DrawField(AssetDragDropField<API::Texture3D>{ "LUT" }, LUT, depth);*/
	});
};