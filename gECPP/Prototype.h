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
		explicit Flags(bool s) : Static(s) {};

		bool Static : 1 = false;
		bool Enabled : 1 = true;
		bool Deletion : 1 = false;
	};

	class IComponentManager;
	template<class T> class LinkedList;
	template<class T> class Manager;
	template<class T> class Managed;

}

template<class T = void*> struct Serializable;