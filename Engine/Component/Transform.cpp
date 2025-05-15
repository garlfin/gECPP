//
// Created by scion on 8/25/2023.
//

#include "Transform.h"

#include <Window.h>
#include <Entity/Entity.h>

namespace gE
{
	mat4 Transform::GetParentTransform() const
	{
		Entity* parent = GetOwner().GetParent();
		return parent ? parent->GetTransform()._model : mat4(1.f);
	}

	void Transform::OnUpdate(float)
	{
		if(const Entity* parent = GetOwner().GetParent())
			_flags |= parent->GetTransform()._flags & TransformFlags::RenderInvalidated;

		_previousModel = _model;

		if(!(bool)(_flags & TransformFlags::RenderInvalidated)) return;

		_model = GetParentTransform() * _transform.ToMat4();
		_globalTransform = Decompose(_model);
	}

	void Transform::OnRender(float, Camera*)
	{
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

	TransformData TransformData::operator*(float b) const
	{
		return
		{
			Location * b,
			Scale * b,
			slerp(glm::identity<quat>(), Rotation, b)
		};
	}

	TransformData TransformData::operator+(const TransformData& b) const
	{
		return
		{
			Location + b.Location,
			Scale + b.Scale,
			Rotation * b.Rotation,
		};
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

	mat4 TransformData::ToMat4() const
	{
		mat4 model = translate(mat4(1.0), Location);
		model *= toMat4(Rotation);
		model = scale(model, Scale);

		return model;
	}

#ifdef GE_ENABLE_IMGUI
	void TransformData::OnEditorGUI(u8 depth)
	{
		DrawField(ScalarField<float>{ "Position" }, Position, depth);
		DrawField(ScalarField<float>{ "Rotation" }, Rotation, depth);
		DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, Scale, depth);
	}
#endif

	Transform::Transform(Entity* o, const TransformData& d) :
		Component(o, &GetWindow().GetTransforms()),
	    _model(1.0)
	{
		Set(d);
	}

	void Transform::IDeserialize(istream& in, SETTINGS_T s)
	{
		SetManager(&GetWindow().GetTransforms());

		Read(in, _transform.Position);
		Read(in, _transform.Rotation);
		Read(in, _transform.Scale);

		_flags = TransformFlags::All;
	}

	void Transform::ISerialize(ostream& out) const
	{
		Write(out, _transform.Position);
		Write(out, _transform.Rotation);
		Write(out, _transform.Scale);
	}

	REFLECTABLE_ONGUI_IMPL(Transform,
	{
		DrawField(ScalarField<float>{ "Position" }, *this, depth, &Transform::GetPosition_, &Transform::SetPosition_);
		DrawField(ScalarField<float>{ "Rotation" }, *this, depth, &Transform::GetRotation_, &Transform::SetRotation_);
		DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, *this, depth, &Transform::GetScale_, &Transform::SetScale_);
	});
	REFLECTABLE_FACTORY_IMPL(Transform);

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
