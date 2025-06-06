//
// Created by scion on 8/25/2023.
//

#pragma once

#include <cstddef>

#include "Core/Macro.h"

namespace gE
{
	class Window;
	class Entity;
	class Component;
	class Behavior;
	class Camera;

	template<class T> class TypedComponent;
	template<class T> class TypedBehavior;

	enum class EntityFlags
	{
		Static = 1,
		Enabled = 1 << 1,
		Deletion = 1 << 2,
		Internal = 1 << 3
	};

	ENUM_OPERATOR(EntityFlags, &);
	ENUM_OPERATOR(EntityFlags, |);

	class IComponentManager;
	template<class T> class LinkedList;
	template<class T> class Manager;
	template<class T> class Managed;

	using ComponentUpdateFunction = void (Component::*)(float);
	using ComponentRenderFunction = void (Component::*)(float, Camera*);

}

template<class T = std::nullptr_t>
struct Reflectable;

template<class T = std::nullptr_t>
struct Serializable;