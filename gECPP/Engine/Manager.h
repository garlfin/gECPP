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

	class Manager : protected std::vector<Updateable*>
	{
	 public:
		Manager() = default;

		inline virtual void Register(Updateable* t) { push_back(t); }
		inline virtual void Remove(Updateable* t) { RemoveFirstFromVec(*this, t); }

		NODISCARD ALWAYS_INLINE size_t Size() const { return size(); }

	 protected:
		virtual void OnUpdate(float delta)
		{
			_updateTick++;
			for(Updateable* t: *this)
			{
				if(t->_updateTick == _updateTick) continue; // Prevents a component from running twice in a frame
				if (t->GetFlags().Deletion)
					t->OnDestroy();
				else
					t->OnUpdate(delta);
				t->_updateTick = _updateTick;
			}
		}

		virtual void OnRender(float delta)
		{
			_renderTick++;
			for(Updateable* t: *this)
			{
				if(t->_renderTick == _renderTick) continue; // Prevents a component from running twice in a frame
				t->OnRender(delta);
				t->_renderTick = _renderTick;
			}
		}

		using std::vector<Updateable*>::operator[];

	 protected:
		
		inline bool Contains(Updateable* v) { return std::find(begin(), end(), v) != end(); }
		u64 _updateTick = 0, _renderTick = 0;
	};

	template<class T>
	class TypedManager : public Manager
	{
	 public:
		TypedManager() = default;

		inline virtual void Register(T* t) { Manager::Register(t); }
		inline virtual void Remove(T* t) { Manager::Remove(t); }

		using Manager::Size;
		using Manager::OnUpdate;
		using Manager::OnRender;

		NODISCARD ALWAYS_INLINE T* operator[](size_t i) const { return (T*) Manager::operator[](i); }
	};

	template<class T>
	using ComponentManager = TypedManager<T>;

	template<class T>
	using EntityManager = TypedManager<T>;
}
