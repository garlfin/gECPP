//
// Created by scion on 12/21/2023.
//

#pragma once

#include "Component.h"
#include <Engine/Manager.h>

namespace gE
{
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
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(T* o) : Behavior(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop
}