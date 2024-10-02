//
// Created by scion on 8/25/2023.
//

#pragma once

#include <Engine/Utility/Manager.h>

#include "Prototype.h"
#include "Engine/Utility/RelativePointer.h"

namespace gE
{
	class Component : public Managed<Component>
	{
	 public:
		explicit Component(Entity* o, IComponentManager* = nullptr);

		GET_CONST(Entity&, Owner, *_owner);
		GET_CONST(Window&, Window, _window);

		virtual void OnInit() { };
		virtual void OnUpdate(float d) { };
		virtual void OnRender(float d, Camera* camera) { };
		virtual void OnDestroy() { };

		~Component() override = default;

	 private:
		Window& _window;
		RelativePointer<Entity> _owner;
	};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

	template<class T>
	class TypedComponent : public Component
	{
	 public:
		inline explicit TypedComponent(T* o) : Component(o) { };

		GET_CONST(T*, Owner, (T*) Component::GetOwner());
	};

#pragma clang diagnostic pop

	class IComponentManager : public Manager<Managed<Component>>
	{
	 public:
		using Manager::Manager;

		void OnUpdate(float d) override
		{
			for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
				(**i)->OnInit();

			List.MergeList(InitializationList);

			for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
				(**i)->OnUpdate(d);
		}

		void OnRender(float d, Camera* camera) override
		{
			for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
				(**i)->OnRender(d, camera);
		}

		ALWAYS_INLINE void OnRender(float d) { OnRender(d, nullptr); }

		friend class Component;

		~IComponentManager() override = default;

	 protected:
		void OnRegister(Managed<Component>& t) override { InitializationList.Add(t.GetIterator()); };

		LinkedList<Managed<Component>> InitializationList;
	};

	template<class T>
	class ComponentManager : public IComponentManager
	{
	public:
		using IComponentManager::IComponentManager;
	};
}
