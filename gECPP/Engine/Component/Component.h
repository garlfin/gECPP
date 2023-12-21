//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"

namespace gE
{
	class Component : public Managed<Component, IComponentManager>
	{
	 public:
		explicit Component(Entity* o, IComponentManager* manager);

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

	class Behavior : public Component
	{
	 public:
		explicit Behavior(Entity* o);

		void OnUpdate(float d) override { }
		void OnRender(float d) override { }
		void OnDestroy() override { }
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

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(T* o) : Behavior(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop
}