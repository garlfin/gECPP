//
// Created by scion on 8/26/2023.
//

#include "Entity.h"

#include <Engine/Component/Behavior.h>
#include <Engine/Component/Component.h>

#include "Engine/Window.h"

namespace gE
{
	Entity::Entity(Window* w, LayerMask layers, Flags flags, Entity* parent) :
		_window(w), _parent(parent), _flags(flags), _layers(layers), _transform(this)
	{
		if(flags.Static) _layers |= LayerMask::Static;
		if(parent) parent->_children.push_back(this);
	}

	void Entity::Destroy(bool flagChildren)
	{
		_flags.Deletion = true;

		RemoveFirstFromVec(_parent->_children, this);

		std::vector stack{ this };

		if(flagChildren)
			while(!stack.empty())
			{
				Entity* back = stack.back();
				back->_flags.Deletion = true;
				stack.pop_back();

				for(Entity* child: back->GetChildren())
					stack.push_back(child);
			}
		else
			for(Entity* child: _children)
			{
				child->_parent = GetParent();
				GetParent()->_children.push_back(child);
			}
	}

	Component::Component(Entity* o, IComponentManager* m) : Managed(m, *this),
		 _window(o->GetWindow()), _owner(o)
	{}

	void IComponentManager::OnUpdate(float d)
	{
		OnInit();

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnUpdate(d);
	}

	void IComponentManager::OnFixedUpdate(float d)
	{
		OnInit();

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnFixedUpdate(d);
	}

	void IComponentManager::OnRender(float d, Camera* camera)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnRender(d, camera);
	}

	void IComponentManager::OnInit()
	{
		for(ITER_T* i = InitializationList.GetFirst(); i; i = i->GetNext())
			(**i)->OnInit();

		List.MergeList(InitializationList);
	}

	Behavior::Behavior(Entity* o) : Component(o, &o->GetWindow().GetBehaviors())
	{

	}
}
