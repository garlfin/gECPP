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
		explicit Component(Entity* o);;

		GET_CONST_VALUE(Entity*, Owner, _owner);
		GET_CONST_VALUE(Window*, Window, Window);
		GET_CONST_VALUE(Flags, Flags, Flags);
		GET_CONST_VALUE(u64, UpdateTick, _updateTick);
		GET_CONST_VALUE(u64, RenderTick, _renderTick);

		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() { };

		virtual ~Component() = default;

	 protected:
		Window* const Window;
		Flags& Flags;

	 private:
		Entity* const _owner;
		u64 _updateTick = 0, _renderTick = 0;

		template<class T> requires IsComponent<T>
		friend class Manager;
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
		inline explicit TypedComponent(gE::Entity* o) : Component(o) { };

		GET_CONST_VALUE(T*, Owner, (T*) Component::GetOwner());
	};

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(gE::Entity* o) : Behavior(o) { };

		GET_CONST_VALUE(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop
}