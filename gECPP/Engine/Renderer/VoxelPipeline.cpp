//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"

namespace gE::VoxelPipeline
{
	Buffers::Buffers(gE::Window* window) : _voxelBuffer(window)
	{
	}

	Target3D::Target3D(Camera3D& camera) : RenderTarget<Camera3D>(camera),
		_color(&camera.GetWindow(), { VoxelPipeline::ColorFormat, camera.GetSize() }),
		_data(&camera.GetWindow(), { VoxelPipeline::DataFormat, camera.GetSize() })
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
	}

	void Target3D::RenderPass(float, Camera*)
	{
		// TODO
	}
}

