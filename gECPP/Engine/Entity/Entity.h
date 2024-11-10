//
// Created by scion on 8/26/2023.
//

#pragma once

#include <vector>
#include <Engine/Utility/Binary.h>
#include <Engine/Component/Transform.h>
#include <Engine/Component/Layer.h>

#include "Prototype.h"

namespace gE
{
	class EntityManager;

 	class Entity : public Managed<Entity>
	{
	 public:
		explicit Entity(Window*, LayerMask layers = LayerMask::All, EntityFlags = EntityFlags(), Entity* = nullptr);

		void Destroy(bool flagChildren = true);

		GET(Transform&, Transform, _transform);
		GET_CONST(const std::vector<Entity*>&, Children, _children);
		GET_CONST(Window&, Window, *_window);
		GET_CONST(Entity*, Parent, _parent);
		GET_CONST(EntityFlags, Flags, _flags);
 		GET_CONST(LayerMask, Layer, _layers);

 		friend class EntityManager;

	 private:
		Window* const _window = nullptr;
		Entity* _parent = nullptr;

		EntityFlags _flags;
		LayerMask _layers;
 		u8 _depth = DEFAULT;

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
