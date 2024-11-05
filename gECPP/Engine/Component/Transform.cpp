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
		Entity* parent = GetOwner().GetParent();
		return parent ? parent->GetTransform()._model : glm::mat4(1.f);
	}

	void Transform::OnUpdate(float)
	{
		Entity* parent = GetOwner().GetParent();

		_flags.UpdateInvalidated |= parent && parent->GetTransform()._flags.UpdateInvalidated;

		if(!_flags.Initialized && !_flags.UpdateInvalidated) return;

		_model = GetParentTransform() * _transform.ToMat4();
		Decompose(_model, _globalTransform.Location, _globalTransform.Rotation, _globalTransform.Scale);
	}

	void Transform::OnRender(float, Camera*)
	{
		_previousModel = _model;
	}

	Transform::Transform(Entity* o) : Component(o, &o->GetWindow().GetTransforms()),
		_model(1.0)
	{
	}

	TransformData TransformData::mix(const TransformData& a, const TransformData& b, float factor)
	{
		return TransformData
		{
			glm::mix(a.Location, b.Location, factor),
			glm::mix(a.Scale, b.Scale, factor),
			slerp(a.Rotation, b.Rotation, factor)
		};
	}

	glm::mat4 TransformData::ToMat4() const
	{
		glm::mat4 model = translate(glm::mat4(1.0), Location);
		model *= toMat4(Rotation);
		model = scale(model, Scale);

		return model;
	}

	Transform::Transform(Entity* o, const TransformData& d) : Component(o, &GetWindow().GetTransforms()),
	                                                          _model(1.0)
	{
		Set(d);
	}

	void TransformManager::OnUpdate(float d)
	{
		const EngineFlags flags = GetWindow()->EngineState;

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnInit();

		for(ITER_T* i = InitializationList.GetFirst(); i;)
		{
			ITER_T* next = i->GetNext();

			Entity* parent = (**i)->GetOwner().GetParent();
			if(parent)
				List.Insert(*i, parent->GetTransform().GetIterator());
			else List.Add(*i);

			i = next;
		}

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			((***i).*flags.UpdateFunction)(d);
	}
}
