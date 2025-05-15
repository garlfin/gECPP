//
// Created by scion on 12/21/2023.
//

#pragma once

#include <Component/Component.h>

namespace gE
{
	class Behavior : public Component
	{
		SERIALIZABLE_PROTO_ABSTRACT("BHVR", 0, Behavior, Component);

	public:
		explicit Behavior(Entity* o);
	};

	template<class T>
	class TypedBehavior : public Behavior
	{
	 public:
		inline explicit TypedBehavior(T* o) : Behavior(o) { };

		GET_CONST(T&, Owner, (T&) Behavior::GetOwner());
	};
}