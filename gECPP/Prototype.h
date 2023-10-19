//
// Created by scion on 8/25/2023.
//

#pragma once

#include <GL/GL.h>

namespace gE
{
	class Window;
	class Entity;

	class Component;
	class Behavior;

	template<class T> class TypedComponent;
	template<class T> class TypedBehavior;

	struct Flags
	{
		bool Enabled: 1 = true;
		bool Deletion: 1 = false;
		u8 Layer: 6 = 0;
	};
}