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
		VoxelCapture(Window* w, u16 resolution, float size, Entity* p = nullptr);

		void Update();

		GET_CONST_VALUE(GL::Texture3D*, Color, _camera.GetAttachment(0));
		GET_CONST_VALUE(GL::Texture3D*, Data, _camera.GetAttachment(1));

	 private:
		Camera3D _camera;
		float _size;
		u16 _resolution;
	};
}