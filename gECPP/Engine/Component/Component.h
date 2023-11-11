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

		GET_CONST_VALUE(Entity*, Owner, _owner);
		GET_CONST_VALUE(Window&, Window, *_window);

		void OnUpdate(float d) override { }
		void OnRender(float d) override { }
		void OnDestroy() override { };

		~Component() override = default;

	 private:
		Window* _window;
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

		~Behavior() override;
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(T* o, Manager* manager = nullptr) : Component(o, manager) { };

		GET_CONST_VALUE(T*, Owner, (T*) Component::GetOwner());
	};

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(T* o) : Behavior(o) { };

		GET_CONST_VALUE(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop
}