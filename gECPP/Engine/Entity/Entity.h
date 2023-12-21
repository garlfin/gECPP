//
// Created by scion on 8/26/2023.
//

#pragma once

#include "GL/Binary/Binary.h"
#include "Prototype.h"
#include "Engine/Component/Transform.h"
#include <vector>

namespace gE
{
	class Entity
	{
	 public:
		explicit Entity(Window*, Flags = Flags(), Entity* = nullptr);

		void Destroy(bool flagChildren = true);

		GET(Transform&, Transform, _transform);
		GET_CONST(const std::vector<Entity*>&, Children, _children);
		GET_CONST(Window&, Window, *_window);
		GET_CONST(Entity*, Parent, _parent);
		GET_CONST(Flags, Flags, _flags);

	 private:
		Window* const _window = nullptr;
		Entity* _parent = nullptr;
		Flags _flags;
		std::vector<Entity*> _children;

		Transform _transform = Transform(this);
	};

	typedef Entity Empty;
}