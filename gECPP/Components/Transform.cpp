//
// Created by scion on 8/25/2023.
//

#include "Transform.h"
#include <Entity/Entity.h>
#include <Window.h>

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

gl::mat4 gE::Transform::GetParentTransform()
{
	gE::Entity* parent = GetOwner()->GetParent();
	return parent ? parent->GetTransform()._model : gl::mat4::Identity();
}

void gE::Transform::Set(const gE::Transform& d)
{
	Location = d.Location;
	Scale = d.Scale;
	Rotation = d.Rotation;
}

void gE::Transform::OnRender(float)
{
	_model = GetParentTransform();
	_model *= gl::mat4::FromScaleVector(Scale) * Rotation.ToMatrix4() * gl::mat4::FromTranslationVector(Location);
}

gE::Transform::Transform(gE::Entity* o)
	: Component(o)
{
	GetWindow()->GetTransforms().Register(this);
}

gE::Transform::~Transform()
{
	GetWindow()->GetTransforms().Remove(this);
}
