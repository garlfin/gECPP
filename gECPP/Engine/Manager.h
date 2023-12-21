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
		Manager() = default;

		inline virtual void Register(T* t) { VEC_T::push_back(t); }
		inline virtual void Remove(T* t) { RemoveFirstFromVec(*this, t); }

		virtual void OnUpdate(float) {};
		virtual void OnRender(float) {};

		NODISCARD ALWAYS_INLINE size_t Size() const { return VEC_T::size(); }

		using std::vector<T*>::operator[];

	 protected:
		inline bool Contains(T* v) { return std::find(VEC_T::begin(), VEC_T::end(), v) != VEC_T::end(); }
	};

	class IComponentManager : public Manager<Component>
	{
	 public:
		using Manager<Component>::Manager;

		void OnUpdate(float d) override;
		void OnRender(float d) override;
	};

	template<class T>
	class ComponentManager : public IComponentManager
	{
	 public:
		using IComponentManager::IComponentManager;

		using IComponentManager::OnRender;
		using IComponentManager::OnUpdate;
		using IComponentManager::Size;

		inline virtual void Register(T* t) { IComponentManager::Register(t); }
		inline virtual void Remove(T* t) { IComponentManager::Remove(t); }
	};

	template<class I, class T>
	class Managed
	{
	 public:
		Managed(I* obj, T* manager) : _manager(manager), _this(obj) { if(_manager) _manager->Register(obj); }
		~Managed() { if(_manager) _manager->Remove(_this); }

	 private:
		T* _manager;
		I* _this;
	};
}
