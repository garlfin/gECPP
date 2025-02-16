//
// Created by scion on 8/26/2023.
//

#include "Entity.h"

#include <Component/Behavior.h>
#include <Component/Component.h>
#include <Window/Window.h>

namespace gE
{
	Entity::Entity(Window* window, Entity* parent, LayerMask layers, EntityFlags flags) :
		Managed(&window->GetEntities(), *this),
		_window(window), _parent(parent), _flags(flags),
		_layers(layers), _transform(this)
	{
		if(flags.Static) _layers |= LayerMask::Static;
		if(!parent) return;

		_sceneTreeDepth = _parent->_sceneTreeDepth + 1;
		parent->_children.push_back(this);
	}

	void Entity::IOnEditorGUI(u8 depth)
	{
		Editor::DrawField(Field{ "Name"sv }, _name, depth);
		Editor::DrawField(Field{ "Parent"sv }, _parent, depth);
		Editor::DrawField(Field{ "Transform"sv }, _transform, depth);
	}

	void Entity::Destroy(bool flagChildren)
	{
		GetWindow().GetEntities().DestroyEntity(*this, flagChildren);
	}

	void EntityManager::MarkDeletions() const
	{
		for(ITER_T* i = _deletionList.GetFirst(); i; i = i->GetNext())
			(**i)->_flags.Deletion = true;
	}

	void EntityManager::FinalizeDeletions() const
	{
		for(ITER_T* i = _deletionList.GetFirst(); i;)
		{
			const Entity& toBeDeleted = ***i;
			i = i->GetNext();

			delete &toBeDeleted;
		}
	}

	void EntityManager::DestroyEntity(Entity& entity, bool destroyChildren)
	{
		const u8 depth = entity._sceneTreeDepth;

		if(destroyChildren)
		{
			ITER_T* end = &entity.Node;
			for(ITER_T* i = &entity.Node; i && (**i)->_sceneTreeDepth > depth; i = i->GetNext())
			{
				(**i)->_flags.Deletion = true;
				end = i;
			}

			_deletionList.MergeList(List, &entity.Node, end);
		}
		else
		{
			std::vector<Entity*>& children = entity._children;
			if(entity._parent)
				entity._parent->_children.insert(entity._parent->_children.end(), children.begin(), children.end());

			for(ITER_T* i = &entity.Node; i && (**i)->_sceneTreeDepth > depth; i = i->GetNext())
				(**i)->_parent = entity._parent;

			_deletionList.Add(entity.Node);
		}
	}

	Component::Component(Entity* o, IComponentManager* m) :
		Managed(m, *this),
		_window(o->GetWindow()), _owner(o)
	{

	}

	void IComponentManager::OnUpdate(float delta)
	{
		OnInit();

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
		{
			Component& iter = ***i;

			if(iter.GetOwner().GetFlags().Deletion)
				iter.OnDestroy();
			else
				iter.OnUpdate(delta);
		}
	}

	void IComponentManager::OnFixedUpdate(float delta)
	{
		OnInit();

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnFixedUpdate(delta);
	}

	void IComponentManager::OnLateFixedUpdate(float delta)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnLateFixedUpdate(delta);
	}

	void IComponentManager::OnRender(float delta, Camera* camera)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnRender(delta, camera);
	}

	void IComponentManager::OnGUI(float delta)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->OnGUI(delta);
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
