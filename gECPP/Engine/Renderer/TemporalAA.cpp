//
// Created by scion on 11/28/2023.
//

#include "TemporalAA.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	TemporalAA::TemporalAA(Window* window) : PostProcessEffect(window, TAARequirements),
		 _shader(window, "Resource/Shader/taa.comp")
	{

	}

	void TemporalAA::RenderPass(Camera& camera, GL::Texture& in, GL::Texture& out)
	{
		TAA_VELOCITY(camera);
	}
}