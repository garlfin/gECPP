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
	class Entity : public Updateable
	{
	 public:
		explicit Entity(Window*, Entity* parent = nullptr, Manager* manager = nullptr);

		void Destroy(bool flagChildren = true);

		GET(Transform&, Transform, _transform);
		GET_CONST(const std::vector<Entity*>&, Children, _children);
		GET_CONST(Window&, Window, *_window);
		GET_CONST(Entity*, Parent, _parent);

		void OnUpdate(float d) override { };
		void OnRender(float d) override { };
		void OnDestroy() override { };

	 private:
		Window* const _window = nullptr;
		Entity* _parent = nullptr;
		Flags _flags = (Flags) 0;
		std::vector<Entity*> _children;

		Transform _transform = Transform(this);

		friend class Component;
		friend class Manager;
		template<class T> friend class TypedManager;
	};
}