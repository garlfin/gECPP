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

	template<class T>
	class TypedComponent;

	template<class T>
	class TypedBehavior;

	struct Flags
	{
		bool Enabled : 1 = true;
		bool Deletion : 1 = false;
		bool Static : 1 = false;
		//bool FutureUse : 1 = false;
		u8 Layer : 4 = 0;
	};

	template<class T>
	concept IsComponent = requires(T& t)
	{
		t.OnUpdate(0.f);
		t.OnRender(0.f);
		t.OnDestroy();
		t.GetUpdateTick();
		t.GetRenderTick();
	};

	template<class T> requires IsComponent<T>
	class Manager;
}