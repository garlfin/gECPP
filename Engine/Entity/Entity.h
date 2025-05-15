//
// Created by scion on 8/26/2023.
//

#pragma once

#include <vector>
#include <Component/Transform.h>
#include <Core/Binary.h>
#include <Core/Serializable/Serializable.h>

#include "Layer.h"
#include "Prototype.h"

namespace gE
{
	class EntityManager;

	struct EntityCreationSettings
	{
		Window* Window;
		Entity* Parent;
	};
}

template class TypeSystem<const gE::EntityCreationSettings&>;

namespace gE
{
  	class Entity : public Serializable<const EntityCreationSettings&>, public Managed<Entity>
	{
  		SERIALIZABLE_PROTO_ABSTRACT("entity", 0, Entity, Serializable);
  		REFLECTABLE_NAME_PROTO();

	public:
		explicit Entity(Window*, Entity* = nullptr, LayerMask layers = LayerMask::All, EntityFlags = DEFAULT);

		void Destroy(bool flagChildren = true);

		GET(Transform&, Transform, _transform);
		GET_CONST(const std::vector<Entity*>&, Children, _children);
		GET_CONST(Window&, Window, *_window);
		GET_CONST(Entity*, Parent, _parent);
		GET_CONST(EntityFlags, Flags, _flags);
 		GET_CONST(LayerMask, Layer, _layers);
  		GET_CONST(u8, TreeDepth, _sceneTreeDepth);
  		GET_SET(std::string&, Name, _name);

 		friend class EntityManager;

	private:
		Window* _window = nullptr;
		Entity* _parent = nullptr;
  		std::string _name = "Entity";

		EntityFlags _flags;
		LayerMask _layers;
 		u8 _sceneTreeDepth = DEFAULT;

		std::vector<Entity*> _children;

		Transform _transform;
	};

	typedef Entity Empty;

	class EntityManager : public Manager<Managed<Entity>>
	{
	public:
		using Manager::Manager;

		void MarkDeletions() const;
		void FinalizeDeletions() const;
		void DestroyEntity(Entity&, bool destroyChildren = true);

	private:
		LinkedList<Managed<Entity>> _deletionList;
	};
}