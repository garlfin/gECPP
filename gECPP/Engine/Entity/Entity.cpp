//
// Created by scion on 8/26/2023.
//

#include "Entity.h"
#include "Engine/Component/Component.h"
#include "Engine/Window.h"

namespace gE
{
	Entity::Entity(Window* w, Entity* o) :
		_window(w), _parent(o),
		_transform(this)
	{

	}

	Behavior::Behavior(Entity* o)
		: Component(o)
	{
		GET_WINDOW()->GetBehaviors().Register(this);
	}

	Behavior::~Behavior()
	{
		GET_WINDOW()->GetBehaviors().Remove(this);
	}

	Flags Component::GetFlags() const
	{
		return Owner()->GetFlags();
	}
}
