//
// Created by scion on 8/25/2023.
//

#pragma once

#include <cstddef>

namespace gE
{
	class Window;
	class Entity;
	class Component;
	class Behavior;
	class Camera;

	template<class T> class TypedComponent;
	template<class T> class TypedBehavior;

	struct EntityFlags
	{
		EntityFlags() = default;
		explicit EntityFlags(bool s) : Static(s) {};

		bool Static : 1 = false;
		bool Enabled : 1 = true;
		bool Deletion : 1 = false;
	};

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