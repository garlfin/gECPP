//
// Created by scion on 9/19/2023.
//

#include <Window.h>

#define MODE_TAA_COMBINE 0
#define MODE_TAA_DOWNSAMPLE 1
#define MODE_TAA_VELOCITY 2
#define MODE_TAA_COPY 3

namespace gE::SDFPipeline
{
	Buffers::Buffers(Window* window) :
		_sdfBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic, true)
	{
		_sdfBuffer.Bind(API::BufferBaseTarget::Uniform, 4);
	}

	Target3D::Target3D(SDFCapture& capture, Camera3D& camera) :
		RenderTarget(capture, camera),
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

	void Target3D::Resize()
	{
	}
}

