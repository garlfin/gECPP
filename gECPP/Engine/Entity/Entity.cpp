//
// Created by scion on 8/26/2023.
//

#include "Entity.h"
#include "Engine/Component/Component.h"
#include "Engine/Window.h"

namespace gE
{
	Entity::Entity(Window* w, Entity* o, const char* n) :
		_window(w), _parent(o), _name(n ? strdup(n) : nullptr),
		_transform(this)
	{

	}

	Window* Component::GetWindow() const
	{
		return _entity->GetWindow();
	}

	Behavior::Behavior(Entity* o)
		: Component(o)
	{
		GetWindow()->GetBehaviors().Register(this);
	}

	Behavior::~Behavior()
	{
		GetWindow()->GetBehaviors().Remove(this);
	}
}
