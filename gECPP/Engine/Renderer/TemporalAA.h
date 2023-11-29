//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Component/CameraPostProcess.h>
#include <GL/Shader/Shader.h>

namespace gE
{
	class TemporalAA : public PostProcessEffect
	{
	 public:
		TemporalAA(Window*);
	 private:
		GL::Shader _shader;
	};
}
