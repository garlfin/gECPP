//
// Created by scion on 7/14/2024.
//

#pragma once

#include <Component/Camera/Camera.h>
#include <Core/Pipeline/SDFPipeline.h>

namespace gE
{
	class SDFCapture : public Entity
	{
	 public:
		SDFCapture(Window* window, u16 resolution);

		void GetGPUSDFScene(GPU::SDFScene& scene);

		GET(Camera3D&, Camera, _camera);
		GET(SDFPipeline::Target3D&, Target, _target);
		GET(API::Texture3D&, Color, _target.GetColor());

	 private:
		Camera3D _camera;
		SDFPipeline::Target3D _target;
	};
}