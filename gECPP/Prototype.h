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
	class Camera;

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

	class Manager;

	template<class T>
	class TypedManager;

	class Updateable
	{
	 public:
		Updateable(Manager* manager, Flags& flags);

		GET_CONST(Flags, Flags, _flags);

		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		virtual ~Updateable();

	 private:
		Manager* _manager;
		Flags& _flags;

		friend class Manager;
		template<class T> friend class TypedManager;
	};

	class IRenderTarget;

	template<class T>
	class RenderTarget;
}