//
// Created by scion on 9/30/2023.
//

#pragma once

#include "Engine/Component/Camera/Camera.h"
#include <Engine/Renderer/VoxelPipeline.h>

namespace gE
{
	class VoxelCamera : public Camera3D
	{
	 public:
		VoxelCamera(Entity*, TARGET_TYPE&, u16, float);
		void GetGLVoxelScene(GL::VoxelScene&);

	 private:
		float _size;
	};

	class VoxelCapture : public Entity
	{
	 public:
		VoxelCapture(Window* w, u16 resolution, float size);

		GET(VoxelPipeline::Target3D&, Target, _target);
		GET(GL::Texture3D&, Color, _target.GetColor());
		GET(GL::Texture3D&, Data, _target.GetData());

	 private:
		VoxelCamera _camera;
		VoxelPipeline::Target3D _target;
	};
}