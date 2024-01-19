//
// Created by scion on 8/25/2023.
//

#include "Transform.h"
#include <Engine/Entity/Entity.h>
#include <Engine/Window.h>

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

void gE::Transform::OnUpdate(float)
{
	_previousModel = _model;

	_model = GetParentTransform();
	_model = glm::translate(_model, Location);
	_model *= glm::toMat4(Rotation);
	_model = glm::scale(_model, Scale);

	_globalTransform.Position = _model[3];
	_globalTransform.Scale = glm::vec3
	{
		glm::length(_model[0]),
		glm::length(_model[1]),
		glm::length(_model[2])
	};

	glm::mat3 rotation =
	{
		glm::vec3(_model[0]) / Scale.x,
		glm::vec3(_model[1]) / Scale.y,
		glm::vec3(_model[2]) / Scale.z,
	};

	_globalTransform.Rotation = glm::quat(rotation);
}

gE::Transform::Transform(gE::Entity* o) : Component(o, &o->GetWindow().GetTransforms()),
	_model(1.0)
{}

gE::Transform::Transform(gE::Entity* o, const gE::TransformData& d) : Component(o, &GetWindow().GetTransforms()),
	_model(1.0)
{
	Set(d);
}

void gE::TransformManager::OnUpdate(float delta)
{
	std::vector<Entity*> stack;
	for(Component* t : *this)
	{
		stack.push_back(((Component*) t)->GetOwner());

		while(!stack.empty())
		{
			Entity* back = stack.back();
			back->GetTransform().OnUpdate(delta);
			stack.pop_back();

			for(Entity* child: back->GetChildren())
				stack.push_back(child);
		}
	}
}
