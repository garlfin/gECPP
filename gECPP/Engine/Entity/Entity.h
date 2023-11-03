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
		explicit Entity(Window*, Entity* parent = nullptr);

		void Destroy(bool flagChildren = true);

		GET_CONST_VALUE(Window*, Window, _window);
		GET(Transform&, Transform, _transform);
		GET_CONST_VALUE(Entity*, Parent, _parent);
		GET_CONST_VALUE(Flags, Flags, _flags);
		GET_CONST(std::vector<Entity*>&, Children, _children);

		virtual void OnInit() {};
		virtual void OnUpdate(float delta) {};
		virtual void OnRender(float delta) {};
		virtual void OnDestroy() {};

		virtual ~Entity();

	 private:
		Window* const _window = nullptr;
		Entity* _parent = nullptr;
		Flags _flags = (Flags) 0;
		std::vector<Entity*> _children;

		Transform _transform = Transform(this);
	};
}