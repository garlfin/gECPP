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

	Behavior::Behavior(Entity* o)
		: Component(o)
	{
		Owner()->GetWindow()->GetBehaviors().Register(this);
	}

	Behavior::~Behavior()
	{
		Owner()->GetWindow()->GetBehaviors().Remove(this);
	}
}
