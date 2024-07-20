//
// Created by scion on 7/14/2024.
//

#pragma once

#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/SDFPipeline.h>

namespace gE
{
	class SDFCapture : public Entity
	{
	 public:
		SDFCapture(Window* w, u16 resolution);

		void GetGLSDFScene(GL::SDFScene& scene);

		GET(Camera3D&, Camera, _camera);
		GET(SDFPipeline::Target3D&, Target, _target);
		GET(GL::Texture3D&, Color, _target.GetColor());

	 private:
		Camera3D _camera;
		SDFPipeline::Target3D _target;
	};
}