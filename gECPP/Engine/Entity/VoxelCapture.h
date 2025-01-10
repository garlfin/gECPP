//
// Created by scion on 9/30/2023.
//

#pragma once

#include "Asset/Mesh/Mesh.h"
#include "Engine/Component/Camera/Camera.h"
#include "Engine/Renderer/VoxelPipeline.h"

namespace gE
{
	struct VoxelCaptureSettings
	{
	public:
		u16 Resolution;
		float Size;
		VoxelPipeline::ProbeSettings ProbeSettings;
	};

	class VoxelCapture : public Entity
	{
	 public:
		VoxelCapture(Window* w, VoxelCaptureSettings settings);

		void GetGPUVoxelScene(API::VoxelScene&);

		GET(Camera3D&, Camera, _camera);
		GET(VoxelPipeline::Target3D&, Target, _target);
		GET(API::Texture3D&, Color, _target.GetColor());

	 private:
		Camera3D _camera;
		VoxelPipeline::Target3D _target;
	};
}