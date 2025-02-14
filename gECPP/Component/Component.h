//
// Created by scion on 8/25/2023.
//

#pragma once

#include "Prototype.h"

#include <Utility/Manager.h>
#include <Utility/RelativePointer.h>
#include <Serializable/Reflectable.h>

namespace gE
{
	class Component : public Reflectable, public Managed<Component>
	{
	 public:
		explicit Component(Entity* o, IComponentManager* = nullptr);

		GET_CONST(Entity&, Owner, *_owner);
		GET_CONST(Window&, Window, _window);

		using UpdateFunction = void(Component::*)(float);
		using RenderFunction = void(Component::*)(float, Camera*);

		virtual void OnInit() = 0;

		virtual void OnFixedUpdate(float delta) {};
		virtual void OnLateFixedUpdate(float delta) {};
		virtual void OnUpdate(float delta) { };
		virtual void OnRender(float delta, Camera* camera) {};
		virtual void OnGUI(float delta) {};

		virtual void OnDestroy() {};

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

		virtual void OnFixedUpdate(float delta);
		virtual void OnLateFixedUpdate(float delta);
		virtual void OnUpdate(float delta);
		virtual void OnRender(float delta, Camera* camera);
		virtual void OnGUI(float delta);

		ALWAYS_INLINE void OnRender(float d) { OnRender(d, nullptr); }

		GET_CONST(Window*, Window, _window);

		friend class Component;

		~IComponentManager() override = default;

	 protected:
		void OnInit();
		void OnRegister(Managed<Component>& t) override { InitializationList.Add(t.GetNode()); };

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
