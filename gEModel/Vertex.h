//
// Created by scion on 1/13/2024.
//

#pragma once

#include "GL/Binary/Binary.h"

struct Vertex
{
	Vertex() = default;

	glm::vec3 Position;
	alignas(8) glm::vec2 UV;
	glm::i8vec3 Normal;
	alignas(4) glm::i8vec3 Tangent;
};

struct Face
{
	glm::u32vec3 Triangle;
};
