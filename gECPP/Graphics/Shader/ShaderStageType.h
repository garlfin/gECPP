//
// Created by scion on 9/14/2024.
//

#pragma once

#include <GLAD/glad.h>

namespace GPU
{
	enum class ShaderStageType : GLenum
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER
	};
}