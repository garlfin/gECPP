//
// Created by scion on 8/25/2023.
//

#include "Transform.h"

#include <Window.h>
#include <Entity/Entity.h>

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

		if(parent)
			_flags |= parent->GetTransform()._flags & TransformFlags::RenderInvalidated;

		if(!(bool)(_flags & TransformFlags::RenderInvalidated)) return;

		_model = GetParentTransform() * _transform.ToMat4();
		Decompose(_model, _globalTransform.Location, _globalTransform.Rotation, _globalTransform.Scale);
	}

	void Transform::OnRender(float, Camera*)
	{
		_previousModel = _model;
		_flags &= ~TransformFlags::RenderInvalidated;
	}

	void Transform::OnFixedUpdate(float x)
	{
		_flags &= ~TransformFlags::PhysicsInvalidated;
	}

	Transform::Transform(Entity* o) :
		Component(o, &o->GetWindow().GetTransforms()),
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

	Transform::Transform(Entity* o, const TransformData& d) :
		Component(o, &GetWindow().GetTransforms()),
	    _model(1.0)
	{
		Set(d);
	}

	REFLECTABLE_ONGUI_IMPL(Transform,
	{
		DrawField(ScalarField<float>{ "Position" }, *this, depth, &Transform::GetPosition_, &Transform::SetPosition_);
		DrawField(ScalarField<float>{ "Rotation" }, *this, depth, &Transform::GetRotation_, &Transform::SetRotation_);
		DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, *this, depth, &Transform::GetScale_, &Transform::SetScale_);
	});

	REFLECTABLE_FACTORY_NO_IMPL(Transform);

	void TransformManager::OnUpdate(float delta)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnInit();

		for(ITER_T* i = InitializationList.GetFirst(); i;)
		{
			ITER_T* next = i->GetNext();

			Entity* parent = (**i)->GetOwner().GetParent();
			if(parent)
				List.Insert(*i, parent->GetTransform().GetNode());
			else List.Add(*i);

			i = next;
		}

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnUpdate(delta);
	}
}
