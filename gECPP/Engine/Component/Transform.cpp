//
// Created by scion on 8/25/2023.
//

#include "Transform.h"

#include <Engine/Window.h>
#include <Engine/Entity/Entity.h>

namespace gE
{
	glm::mat4 Transform::GetParentTransform() const
	{
		Entity* parent = GetOwner()->GetParent();
		return parent ? parent->GetTransform()._model : glm::mat4(1.f);
	}

	void Transform::OnUpdate(float)
	{
		Entity* parent = GetOwner()->GetParent();

		_flags.Invalidated |= parent && parent->GetTransform()._flags.Invalidated;
		_flags.PreviousInvalidated = _flags.Invalidated;
		if(!_flags.Invalidated) return;

		_model = GetParentTransform();
		_model = translate(_model, _transform.Location);
		_model *= toMat4(_transform.Rotation);
		_model = scale(_model, _transform.Scale);

		Decompose(_model, _globalTransform.Location, _globalTransform.Rotation, _globalTransform.Scale);
	}

	void Transform::OnRender(float, Camera*)
	{
		_previousModel = _model;
	}

	Transform::Transform(Entity* o) : Component(o, &o->GetWindow().GetTransforms()),
		_model(1.0)
	{}

	Transform::Transform(Entity* o, const TransformData& d) : Component(o, &GetWindow().GetTransforms()),
		_model(1.0)
	{
		Set(d);
	}


	void TransformManager::OnUpdate(float d)
	{
		for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
			(*c)->OnInit();

		for(Managed<Component>* c = InitializationList.GetFirst(); c;)
		{
			Managed<Component>* next = c->GetNext();

			InitializationList.Remove(*c);

			Entity* parent = (*c)->GetOwner()->GetParent();
			if(parent)
				List.Insert(*c, parent->GetTransform());
			else List.Add(*c);

			c = next;
		}

		for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
			(*c)->OnUpdate(d);
	}
}
