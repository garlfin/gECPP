//
// Created by scion on 8/25/2023.
//

#include "Transform.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Window.h"

void gE::Transform::Set(const gE::TransformData& d)
{
	Location = d.Location;
	Scale = d.Scale;
	Rotation = d.Rotation;
}

glm::mat4 gE::Transform::GetParentTransform()
{
	gE::Entity* parent = GetOwner()->GetParent();
	return parent ? parent->GetTransform()._model : glm::mat4(1.f);
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

	_model = glm::translate(_model, Location);
	_model *= glm::toMat4(Rotation);
	_model = glm::scale(_model, Scale);
}

gE::Transform::Transform(gE::Entity* o)
	: Component(o)
{
	Window->GetTransforms().Register(this);
}

gE::Transform::~Transform()
{
	Window->GetTransforms().Remove(this);
}

