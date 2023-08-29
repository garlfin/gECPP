//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Forward.h>

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

	 private:
		Entity* const _entity;
	};
}