//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"

namespace gE
{
	class Component : public Updateable
	{
	 public:
		explicit Component(Entity* o, Manager* manager);

		GET_CONST(Entity*, Owner, _owner);
		GET_CONST(Window&, Window, _window);

		void OnUpdate(float d) override { }
		void OnRender(float d) override { }
		void OnDestroy() override { };

	 private:
		Window& _window;
		Entity* _owner;

		friend class Manager;
		template<class T> friend class TypedManager;
	};

	class Behavior : public Component
	{
	 public:
		explicit Behavior(Entity* o);

		void OnUpdate(float d) override { }
		void OnRender(float d) override { }
		void OnDestroy() override { }

		~Behavior() override = default;
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(T* o, Manager* manager = nullptr) : Component(o, manager) { };

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