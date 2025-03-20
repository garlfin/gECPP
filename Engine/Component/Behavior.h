//
// Created by scion on 12/21/2023.
//

#pragma once

#include <Component/Component.h>

namespace gE
{
	class Behavior : public Component
	{
		REFLECTABLE_NOIMPL(Component);

	 public:
		explicit Behavior(Entity* o);
	};

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(T* o) : Behavior(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};
}