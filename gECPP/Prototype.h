//
// Created by scion on 8/25/2023.
//

#pragma once
#include <gETF/Prototype.h>
#include "Engine/Math/Math.h"

namespace gE
{
	class Window;
	class Entity;
	class Component;
	class Behavior;
	class Camera;

	template<class T> class TypedComponent;
	template<class T> class TypedBehavior;

	struct Flags
	{
		Flags() = default;
		Flags(bool s, u8 l) : Static(s), Layer(l) {};

		bool Static : 1 = false;
		bool Enabled : 1 = true;
		bool Deletion : 1 = false;
		//bool FutureUse : 1 = false;
		u8 Layer : 4 = 0;
	};


	template<class T> class ComponentManager;
	template<class T> class ManagedList;
	template<class T> class Manager;
	template<class T> class Managed;

}

template<class T = void*> struct Serializable;