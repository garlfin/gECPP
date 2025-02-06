//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"

#include <Utility/Manager.h>
#include <Utility/RelativePointer.h>

namespace gE
{
	class Component : public Managed<Component>
	{
	 public:
		explicit Component(Entity* o, IComponentManager* = nullptr);

		GET_CONST(Entity&, Owner, *_owner);
		GET_CONST(Window&, Window, _window);

		using UpdateFunction = void(Component::*)(float);
		using RenderFunction = void(Component::*)(float, Camera*);

		virtual void OnInit() { };
		virtual void OnFixedUpdate(float d) { };
		virtual void OnUpdate(float d) { };
		virtual void OnRender(float d, Camera* camera) { };
		virtual void OnDestroy() { };

		~Component() override = default;

	 private:
		Window& _window;
		RelativePointer<Entity> _owner;
	};

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(T* o) : Component(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

	class IComponentManager : public Manager<Managed<Component>>
	{
	 public:
		explicit IComponentManager(Window* window) : Manager(), _window(window) {};

		virtual void OnUpdate(float d);
		virtual void OnFixedUpdate(float d);
		virtual void OnRender(float d, Camera* camera);

		ALWAYS_INLINE void OnRender(float d) { OnRender(d, nullptr); }

		GET_CONST(Window*, Window, _window);

		friend class Component;

		~IComponentManager() override = default;

	 protected:
		void OnInit();
		void OnRegister(Managed<Component>& t) override { InitializationList.Add(t.GetIterator()); };

		LinkedList<Managed<Component>> InitializationList;

	private:
		Window* _window;
	};

	template<class T>
	class ComponentManager : public IComponentManager
	{
	public:
		using IComponentManager::IComponentManager;
	};
}
