//
// Created by scion on 8/26/2023.
//

#include "Entity.h"

#include <Window.h>
#include <Component/Behavior.h>
#include <Component/Component.h>

namespace gE
{
	Entity::Entity(Window* window, Entity* parent, LayerMask layers, EntityFlags flags) :
		Managed(&window->GetEntities(), *this),
		_window(window),
		_parent(parent),
		_flags(flags),
		_layers(layers), _transform(this)
	{
		if((bool) (flags & EntityFlags::Static)) _layers |= LayerMask::Static;
		if(!parent) return;

		_sceneTreeDepth = _parent->_sceneTreeDepth + 1;
		parent->_children.push_back(this);
	}

	void Entity::Destroy(bool flagChildren)
	{
		GetWindow().GetEntities().DestroyEntity(*this, flagChildren);
	}

	void EntityManager::MarkDeletions() const
	{
		for(ITER_T* i = _deletionList.GetFirst(); i; i = i->GetNext())
			(**i)->_flags |= EntityFlags::Deletion;
	}

	void Entity::IDeserialize(istream& in, SETTINGS_T s)
	{
		_window = s.Window;
		_parent = s.Parent;

		SetManager(&_window->GetEntities());
		SetThis(this);

		Read(in, _name);
		Read(in, _flags);
		Read(in, _layers);
		ReadSerializable(in, _transform, ComponentCreationSettings{ s, this });
	}

	void Entity::ISerialize(ostream& out) const
	{
		Write(out, _name);
		Write(out, _flags);
		Write(out, _layers);
		Write(out, _transform);
	}

	REFLECTABLE_ONGUI_IMPL(Entity,
	{
		/*DrawField(Field{ "Name" }, _name, depth);
		DrawField(Field{ "Parent" }, _parent, depth);
		DrawField(Field{ "Transform" }, _transform, depth);*/
	});
	REFLECTABLE_NAME_IMPL(Entity, return _name);
	REFLECTABLE_FACTORY_NO_IMPL(Entity);

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
				(**i)->_flags |= EntityFlags::Deletion;
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

	Component::Component(Entity* owner, IComponentManager* manager) :
		Managed(manager, *this),
		_window(&owner->GetWindow()), _owner(owner)
	{
		GE_ASSERT(owner);
	}

	void Component::IDeserialize(istream& in, SETTINGS_T s)
	{
		_owner = s.Parent;
		_window = &_owner->GetWindow();

		SetThis(this);
	}

	void Component::ISerialize(ostream& out) const {}

	REFLECTABLE_ONGUI_IMPL(Component, );
	REFLECTABLE_FACTORY_NO_IMPL(Component);

	void IComponentManager::OnUpdate(float delta)
	{
		OnInit();

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
		{
			Component& iter = ***i;

			if((bool) (iter.GetOwner().GetFlags() & EntityFlags::Deletion))
			{
				iter.OnDestroy();
				continue;
			}

		#ifdef GE_ENABLE_EDITOR
			if((bool)(iter.GetOwner().GetFlags() & EntityFlags::Internal) || GetWindow()->GetEditor()->GetIsRunning())
		#endif
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

	void Behavior::IDeserialize(istream& in, SETTINGS_T s)
	{
		SetManager(&GetWindow().GetBehaviors());
	}

	void Behavior::ISerialize(ostream& out) const {}

	REFLECTABLE_ONGUI_IMPL(Behavior, );
	REFLECTABLE_FACTORY_NO_IMPL(Behavior);
}
