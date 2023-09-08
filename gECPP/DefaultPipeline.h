//
// Created by scion on 9/6/2023.
//

#pragma once

#include <GL/Buffer/Buffer.h>
#include "Entity/Camera.h"

#define MAX_OBJECT 64

namespace GL
{
	struct Camera
	{
		gl::vec3 Position;
		gl::vec2 ClipPlanes;
		float FOV;

		mat4 Projection;
		mat4 PreviousViewProjection;
		mat4 View[6];
	};

	struct Scene
	{
		uint InstanceCount;
		mat4 Model[MAX_OBJECT];
		mat3 Normal[MAX_OBJECT];
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

		void UpdateCamera(const class Camera& cam) const;
		ALWAYS_INLINE void UpdateCamera(const GL::Camera& cam) const { Camera.ReplaceData(&cam); };
	};
}

