//
// Created by scion on 9/30/2023.
//

#pragma once

#include <Engine/Component/Camera.h>
#include <Engine/Renderer/VoxelPipeline.h>

namespace gE
{
	class VoxelCapture : public Camera3D
	{
	 public:
		VoxelCapture(Window* w, u16 resolution, float size, Entity* p = nullptr);

		void OnUpdate(float) override;

		GET_CONST(GL::Texture3D*, Color, GetAttachment(0));
		GET_CONST(GL::Texture3D*, Data, GetAttachment(1));

	 private:
		float _size;
		u16 _resolution;
	};
}