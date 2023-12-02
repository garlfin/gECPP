//
// Created by scion on 9/30/2023.
//

#pragma once

#include "Engine/Component/Camera/Camera.h"
#include <Engine/Renderer/VoxelPipeline.h>

namespace gE
{
	class VoxelTarget : public RenderTarget<Camera3D>
	{
	 public:
		explicit VoxelTarget(Camera3D&);

		GET(GL::Texture3D&, Color, _color);
		GET(GL::Texture3D&, Data, _data);

		void RenderPass() override;

	 private:
		GL::Texture3D _color;
		GL::Texture3D _data;
	};

	class VoxelCapture : public Entity
	{
	 public:
		VoxelCapture(Window* w, u16 resolution, float size);

		GET(VoxelTarget&, Target, _target);
		GET(GL::Texture3D&, Color, _target.GetColor());
		GET(GL::Texture3D&, Data, _target.GetData());

		void OnUpdate(float) override;

	 private:
		Camera3D _camera;
		VoxelTarget _target;
		float _size;
	};
}