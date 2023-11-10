//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include <algorithm>
#include "Component/Component.h"

namespace gE
{
	class Window;

	template<class T> requires IsComponent<T>
	class Manager : protected std::vector<T*>
	{
	 public:
		Manager() = default;

		inline virtual void Register(T* t) { VEC_T::push_back(t); }
		inline virtual void Remove(T* t) { RemoveFirstFromVec(*this, t); }

		virtual void OnUpdate(float delta)
		{
			_updateTick++;
			for(T* t: *this)
			{
				if (t->GetFlags().Deletion)
					t->OnDestroy();
				else
					t->OnUpdate(delta);
			}
		}

		virtual void OnRender(float delta)
		{
			_renderTick++;
			for(T* t: *this)
			{

				t->OnRender(delta);
			}
		}

		NODISCARD ALWAYS_INLINE size_t Size() const { return VEC_T::size(); }

		using std::vector<T*>::operator[];

	 protected:
		typedef std::vector<T*> VEC_T;
		inline bool Contains(T* v) { return std::find(VEC_T::begin(), VEC_T::end(), v) != VEC_T::end(); }
		u64 _updateTick = 0, _renderTick = 0;
	};

	template<class T>
	using ComponentManager = Manager<T>;

	// TODO: Consider moving into Component
	template<class T>
	class Registry
	{
	 public:
		Registry(T* t, Manager<T>* m) : _t(t), _manager(m) { m->Register(t); }
		~Registry() { _manager->Remove(_t); }
	 private:
		T* _t;
		Manager<T>* _manager;
	};

	template<class T, class I>
	class RegistryPair : public Registry<I>
	{
	 public:
		template<typename... ARGS>
		explicit RegistryPair(Manager<I>* m, ARGS&&... args) : Object(std::forward<ARGS>(args)...), Registry<I>(&Object, m) {};

		ALWAYS_INLINE T* Get() const { return Object; }

		ALWAYS_INLINE const T& operator*() const { return Object; }
		ALWAYS_INLINE T& operator*() { return Object; }
		ALWAYS_INLINE const T* operator->() const { return &Object; }
		ALWAYS_INLINE T* operator->() { return &Object; }

		ALWAYS_INLINE operator const T&() const { return Object; } // NOLINT
		ALWAYS_INLINE operator T&() { return Object; } // NOLINT
		ALWAYS_INLINE operator const T*() const { return &Object; } // NOLINT
		ALWAYS_INLINE operator T*() { return &Object; } // NOLINT

		T Object;
	};
}
