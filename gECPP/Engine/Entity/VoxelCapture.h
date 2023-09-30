//
// Created by scion on 9/30/2023.
//

#pragma once

#include <Engine/Component/Camera.h>
#include <Engine/Renderer/VoxelPipeline.h>

namespace gE
{
	class VoxelCapture : public Entity
	{
	 public:
	 private:
		Camera3D _camera;
	};
}