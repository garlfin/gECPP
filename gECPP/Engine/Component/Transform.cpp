//
// Created by scion on 8/25/2023.
//

#include "Transform.h"
#include <Engine/Entity/Entity.h>
#include <Engine/Window.h>

glm::mat4 gE::Transform::GetParentTransform()
{
	gE::Entity* parent = GetOwner()->GetParent();
	return parent ? parent->GetTransform()._model : glm::mat4(1.f);
}

void gE::Transform::OnUpdate(float)
{
	Entity* parent = GetOwner()->GetParent();

	_flags.Invalidated |= parent && parent->GetTransform()._flags.Invalidated;
	_flags.PreviousInvalidated = _flags.Invalidated;
	if(!_flags.Invalidated) return;

	_model = GetParentTransform();
	_model = glm::translate(_model, _transform.Location);
	_model *= glm::toMat4(_transform.Rotation);
	_model = glm::scale(_model, _transform.Scale);

	Decompose(_model, _globalTransform.Location, _globalTransform.Rotation, _globalTransform.Scale);
}

void gE::Transform::OnRender(float, Camera*)
{
	_previousModel = _model;
}

gE::Transform::Transform(gE::Entity* o) : Component(o, &o->GetWindow().GetTransforms()),
	_model(1.0)
{}

gE::Transform::Transform(gE::Entity* o, const gE::TransformData& d) : Component(o, &GetWindow().GetTransforms()),
	_model(1.0)
{
	Set(d);
}


void gE::TransformManager::OnUpdate(float d)
{
	for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
		(*c)->OnInit();

	for(Managed<Component>* c = InitializationList.GetFirst(); c; c = c->GetNext())
	{
		InitializationList.Remove(*c);

		Entity* parent = (*c)->GetOwner()->GetParent();
		if(parent)
			List.Insert(*c, parent->GetTransform());
		else List.Add(*c);
	}

	for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
		(*c)->OnUpdate(d);
}
