//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"
#include "Engine/Entity/VoxelCapture.h"
#include "Engine/Window.h"

#define MODE_TAA_COMBINE 0
#define MODE_TAA_DOWNSAMPLE 1
#define MODE_TAA_VELOCITY 2
#define MODE_TAA_COPY 3

namespace gE::SDFPipeline
{
	Buffers::Buffers(gE::Window* window) : _sdfBuffer(window)
	{
		_sdfBuffer.Bind(API::BufferTarget::Uniform, 4);
	}

	Target3D::Target3D(SDFCapture& capture, Camera3D& camera) :
		RenderTarget<Camera3D>(capture, camera),
		_color(&camera.GetWindow(), { ColorFormat, camera.GetSize() }),
		_sdf(&camera.GetWindow(), { SDFFormat, camera.GetSize() }),
		_sdfBack(&camera.GetWindow(), { SDFFormat, camera.GetSize() })
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
	}

	void Target3D::RenderPass(float d, Camera* camera)
	{
	}

	bool Target3D::Setup(float d, Camera* camera)
	{
		if(!camera) return false;

		return true;
	}

	void Target3D::PostProcessPass(float d)
	{
	}
}

