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
 	class Entity
	{
	 public:
		virtual ~Entity() = default;

		explicit Entity(Window*, LayerMask layers = LayerMask::All, Flags = Flags(), Entity* = nullptr);

		void Destroy(bool flagChildren = true);

		GET(Transform&, Transform, _transform);
		GET_CONST(const std::vector<Entity*>&, Children, _children);
		GET_CONST(Window&, Window, *_window);
		GET_CONST(Entity*, Parent, _parent);
		GET_CONST(Flags, Flags, _flags);
 		GET_CONST(LayerMask, Layer, _layers);

	 private:
		Window* const _window = nullptr;
		Entity* _parent = nullptr;

		Flags _flags;
		LayerMask _layers;

		std::vector<Entity*> _children;

		Transform _transform;
	};

	typedef Entity Empty;
}
