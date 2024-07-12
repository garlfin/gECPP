//
// Created by scion on 9/30/2023.
//

#pragma once

#include "Engine/Component/Camera/Camera.h"
#include "Engine/Renderer/VoxelPipeline.h"

namespace gE
{
	class VoxelCapture : public Entity
	{
	 public:
		VoxelCapture(Window* w, u16 resolution, float size);

		void GetGLVoxelScene(GL::VoxelScene&);

		GET(Camera3D, Camera, _camera);
		GET(VoxelPipeline::Target3D&, Target, _target);
		GET(GL::Texture3D&, Color, _target.GetColor());

	 private:
		Camera3D _camera;
		VoxelPipeline::Target3D _target;
	};
}