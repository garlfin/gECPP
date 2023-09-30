//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"
#define GET_WINDOW() Owner()->GetWindow()

namespace gE
{
	class Component
	{
	 public:
		explicit Component(Entity* o) : _entity(o) {};
		NODISCARD ALWAYS_INLINE Entity* Owner() const { return _entity; }

		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() {};

		virtual ~Component() = default;

	 private:
		Entity* const _entity;
	};

	class Behavior : public Component
	{
	 public:
		explicit Behavior(Entity* o);

		void OnUpdate(float d) override {}
		void OnRender(float d) override {}
		void OnDestroy() override {}

		~Behavior() override;
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(gE::Entity* o) : Component(o) {};

		NODISCARD ALWAYS_INLINE T* Owner() const { (T*) Component::Owner(); }
	};

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(gE::Entity* o) : Behavior(o) {};

		NODISCARD ALWAYS_INLINE T* Owner() const { (T*) Behavior::Owner(); }
	};

#pragma clang diagnostic pop

}