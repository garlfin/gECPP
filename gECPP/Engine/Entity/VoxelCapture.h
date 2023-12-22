//
// Created by scion on 9/30/2023.
//

#pragma once

#include "Engine/Component/Camera/Camera.h"
#include <Engine/Renderer/VoxelPipeline.h>

namespace gE
{
	class VoxelCapture : public Entity
	{
	 public:
		VoxelCapture(Window* w, u16 resolution, float size);

		void GetGLVoxelScene(GL::VoxelScene&);

		GET(VoxelPipeline::Target3D&, Target, _target);
		GET(GL::Texture3D&, Color, _target.GetColor());
		GET(GL::Texture3D&, Data, _target.GetData());

	 private:
		Camera3D _camera;
		VoxelPipeline::Target3D _target;
		float _size;
	};
}