//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Prototype.h>

namespace gE
{
	class Component
	{
	 public:
		explicit Component(Entity* o) : _entity(o) {};
		NODISCARD ALWAYS_INLINE Entity* GetOwner() const { return _entity; }
		NODISCARD Window* GetWindow() const;

		virtual void OnStart() {};
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() {};

		virtual ~Component() = default;

	 private:
		Entity* const _entity;
	};

	// For future development.
	class Behavior : public Component
	{
	 public:
		explicit Behavior(Entity* o);

		void OnStart() override {}
		void OnUpdate(float d) override {}
		void OnRender(float d) override {}
		void OnDestroy() override {}

		~Behavior() override;
	};
}