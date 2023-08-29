//
// Created by scion on 8/26/2023.
//

#include "Entity.h"

namespace gE
{
	Entity::Entity(Window* w, Entity* o, const char* n) : _window(w), _parent(o), _name(n ? strdup(n) : nullptr), _transform(this)
	{

	}

	Window* Component::GetWindow() const
	{
		return _entity->GetWindow();
	}
}
