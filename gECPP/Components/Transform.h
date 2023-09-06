//
// Created by scion on 8/25/2023.
//

#pragma once

#include <GL/Math.h>
#include "Component.h"

namespace gE
{
	struct TransformData
	{
		union
		{
			gl::vec3 Location = gl::vec3(0.f);
			gl::vec3 Position;
		};
		gl::vec3 Scale = gl::vec3(1.f);
		gl::quaternion Rotation = gl::quaternion::identity;
	};

	class Transform : Component
	{
	 public:
		Transform(Entity* o, TransformData& d) : Component(o)
		{
			Set(d);
		}

		explicit Transform(Entity* o) : Component(o) {}
		void SetRotation(const gl::vec3& r);
		void Set(const TransformData& d);

		void OnUpdate(float) override {};
		void OnRender(float) override {};

		union
		{
			gl::vec3 Location = gl::vec3(0.f);
			gl::vec3 Position;
		};
		gl::vec3 Scale = gl::vec3(1.f);
		gl::quaternion Rotation = gl::quaternion::identity;
	};
}
