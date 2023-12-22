//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"
#include <Engine/Manager.h>

namespace gE
{
	class Component : public Managed<Component>
	{
	 public:
		explicit Component(Entity* o, Manager<Component>* = nullptr);

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
		inline explicit TypedComponent(T* o) : Component(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop

	template<class T>
	class ComponentManager : public Manager<Component>
	{
	 public:
		static_assert(std::is_base_of<Component, T>::value);
		using Manager<Component>::Manager;

		void OnUpdate(float d) override { for(Component* c : *this) c->OnUpdate(d); }
		void OnRender(float d) override { for(Component* c : *this) c->OnRender(d); }
	};
}