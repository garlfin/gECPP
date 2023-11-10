//
// Created by scion on 8/26/2023.
//

#include "Entity.h"
#include "Engine/Component/Component.h"
#include "Engine/Window.h"

namespace gE
{
	Entity::Entity(Window* w, Entity* parent) : _window(w), _parent(parent)
	{
		if(parent) parent->_children.push_back(this);
	}

	void Entity::Destroy(bool flagChildren)
	{
		_flags.Deletion = true;

		RemoveFirstFromVec(_parent->_children, this);

		std::vector<Entity*> stack{ this };

		if(flagChildren)
			for(Entity* child: _children)
			{
				child->_parent = GetParent();
				GetParent()->_children.push_back(child);
			}
		else
			while(!stack.empty())
			{
				Entity* back = stack.back();
				back->_flags.Deletion = true;
				stack.pop_back();

				for(Entity* child: back->GetChildren())
					stack.push_back(child);
			}

	}

	Entity::~Entity() = default;

	Behavior::Behavior(Entity* o)
		: Component(o)
	{
		GetWindow().GetBehaviors().Register(this);
	}

	Behavior::~Behavior()
	{
		GetWindow().GetBehaviors().Remove(this);
	}

	Component::Component(Entity* o) : _owner(o), _window(o->_window), Flags(o->_flags)
	{

	}
}
