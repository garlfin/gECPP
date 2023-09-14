//
// Created by scion on 9/6/2023.
//

#pragma once

#include "GL/Buffer/Buffer.h"
#include "Engine/Entity/Camera.h"

#define MAX_OBJECT 64

#define GL_ALIGN alignas(16)

namespace GL
{
	struct Camera
	{
		glm::vec3 Position;
		GL_ALIGN glm::vec2 ClipPlanes;
		float FOV;


		GL_ALIGN glm::mat4 Projection;
		glm::mat4 PreviousViewProjection;
		glm::mat4 View[6];
	};

	struct Scene
	{
		uint InstanceCount;
		GL_ALIGN glm::mat4 Model[MAX_OBJECT];
		glm::mat3x4 Normal[MAX_OBJECT]; // for alignment purposes.
	};
}

namespace gE
{
	class Window;

	struct DefaultPipelineBuffers
	{
		explicit DefaultPipelineBuffers(Window* window);

		const GL::Buffer<GL::Scene> Scene;
		const GL::Buffer<GL::Camera> Camera;

		ALWAYS_INLINE void UpdateCamera(const GL::Camera& cam) const { Camera.ReplaceData(&cam); };
	};
}

