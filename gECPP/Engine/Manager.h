//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <vector>
#include <algorithm>
#include <Engine/Component/Component.h>

namespace gE
{
	class Window;

	template<class T>
	class Manager : protected std::vector<T*>
	{
	 public:
		typedef std::vector<T*> VEC_T;

		Manager() = default;

		Manager(const Manager&) = delete;
		Manager(Manager&&) noexcept = delete;
		Manager<T>& operator=(const Manager&) = delete;
		Manager<T>& operator=(Manager&&) noexcept = delete;

		inline virtual void Register(T* t) { VEC_T::push_back(t); }
		inline virtual void Remove(T* t) { RemoveFirstFromVec(*this, t); }

		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;

		NODISCARD ALWAYS_INLINE size_t Size() const { return VEC_T::size(); }

		using std::vector<T*>::operator[];

	 protected:
		inline bool Contains(T* v) { return std::find(VEC_T::begin(), VEC_T::end(), v) != VEC_T::end(); }
	};

	template<class T>
	class ComponentManager : public Manager<T>
	{
	 public:
		using Manager<T>::Manager;

		ALWAYS_INLINE operator Manager<Component>&() { return *(Manager<Component>*) this; }
		ALWAYS_INLINE operator const Manager<Component>&() const { return *(const Manager<Component>*) this; }

		void OnUpdate(float d) override { for(Component* c : *this) c->OnUpdate(d); }
		void OnRender(float d) override { for(Component* c : *this) c->OnRender(d); }
	};

	template<class T>
	class Managed
	{
	 public:
		explicit inline Managed(T& t, Manager<T>* m = nullptr) : _t(t), _manager(m) { if(m) m->Register(&t); }
		inline Managed(T& t, Manager<T>& m) : Managed(t, &m) {}

		inline ~Managed() { if(_manager) _manager->Remove(&_t); }

	 private:
		T& _t;
		Manager<T>* _manager;
	};
}