//
// Created by scion on 8/25/2023.
//

#include "Transform.h"

void gE::Transform::SetRotation(const gl::vec3& r)
{
	Rotation = gl::quaternion::FromEulerAngles(r);
}

void gE::Transform::Set(const gE::TransformData& d)
{
	Location = d.Location;
	Scale = d.Scale;
	Rotation = d.Rotation;
}
