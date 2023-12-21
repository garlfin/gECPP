//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"

namespace gE
{
	class Component
	{
	 public:
		explicit Component(Entity* o);

		GET_CONST(Entity*, Owner, _owner);
		GET_CONST(Window&, Window, _window);

		virtual void OnUpdate(float d) { }
		virtual void OnRender(float d) { }
		virtual void OnDestroy() { };

		virtual ~Component() = default;

	 private:
		Window& _window;
		Entity* _owner;
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(T* o, IComponentManager* manager = nullptr) : Component(o, manager) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop
}