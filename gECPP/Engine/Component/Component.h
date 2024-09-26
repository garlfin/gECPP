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
		explicit Component(Entity* o, Manager<Component>* = nullptr);

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

	template<class T>
	class ComponentManager : public Manager<Component>
	{
	 public:
		static_assert(std::is_base_of<Component, T>::value);
		using Manager<Component>::Manager;

		void OnUpdate(float d) override
		{
			for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
				(*c)->OnInit();

			List.MergeList(InitializationList);

			for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
				(*c)->OnUpdate(d);
		}

		void OnRender(float d, Camera* camera) override
		{
			for(Managed<Component>* c = List.GetFirst(); c; c = c->GetNext())
				(*c)->OnRender(d, camera);
		}

		ALWAYS_INLINE void OnRender(float d) { OnRender(d, nullptr); }

		~ComponentManager() override = default;

	 protected:
		ManagedList<Component> InitializationList;

		void Register(Component& t) override
		{
			InitializationList.Add(t);
		}
	};
}
