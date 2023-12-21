//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <vector>
#include <algorithm>

namespace gE
{
	class Window;

	template<class T>
	class Manager : protected std::vector<T*>
	{
	 public:
		typedef std::vector<T*> VEC_T;
		typedef T I;

		Manager() = default;
		Manager(const Manager&) = delete;
		Manager(Manager&&) = delete;

		inline virtual void Register(T* t) { VEC_T::push_back(t); }
		inline virtual void Remove(T* t) { RemoveFirstFromVec(*this, t); }

		virtual void OnUpdate(float) {};
		virtual void OnRender(float) {};

		NODISCARD ALWAYS_INLINE size_t Size() const { return VEC_T::size(); }

		using std::vector<T*>::operator[];

	 protected:
		inline bool Contains(T* v) { return std::find(VEC_T::begin(), VEC_T::end(), v) != VEC_T::end(); }
	};

	class IEntityManager : public Manager<Entity>
	{
	 public:
		using Manager<Entity>::Manager;
	};

	class IComponentManager : public Manager<Component>
	{
	 public:
		using Manager<Component>::Manager;

		void OnUpdate(float d) override;
		void OnRender(float d) override;
	};

	template<class T>
	class EntityManager : public IEntityManager
	{
	 public:
		using IEntityManager::IEntityManager;

		inline virtual void Register(T* t) { IEntityManager::Register(t); }
		inline virtual void Remove(T* t) { IEntityManager::Remove(t); }
	};

	template<class T>
	class ComponentManager : public IComponentManager
	{
	 public:
		using IComponentManager::IComponentManager;

		inline virtual void Register(T* t) { IComponentManager::Register(t); }
		inline virtual void Remove(T* t) { IComponentManager::Remove(t); }
	};

	template<class T, class I>
	class Managed
	{
	 public:
		Managed(T* manager) : _manager(manager) { if(_manager) _manager->Register((I*) this); }
		~Managed() { if(_manager) _manager->Remove((I*) this); }

	 private:
		T* _manager;
	};
}
