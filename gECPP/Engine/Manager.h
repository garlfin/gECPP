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

		NODISCARD ALWAYS_INLINE size_t Size() const { return size(); }
		using std::vector<Updateable*>::operator[];

		virtual void OnUpdate(float delta)
		{
			for(Updateable* t: *this)
				if(t->GetFlags().Deletion)
					t->OnDestroy();
				else
					t->OnUpdate(delta);
		}

		virtual void OnRender(float delta)
		{
			for(Updateable* t: *this)
				t->OnRender(delta);
		}

	 protected:
		inline void Register(Updateable* t) { push_back(t); }
		inline virtual void Remove(Updateable* t) { RemoveFirstFromVec(*this, t); }

		inline bool Contains(Updateable* v) { return std::find(begin(), end(), v) != end(); }

		friend class Updateable;
	};

	template<class T>
	class TypedManager : public Manager
	{
	 public:
		TypedManager() = default;

		inline virtual void Register(T* t) { Manager::Register(t); }
		inline virtual void Remove(T* t) { Manager::Remove(t); }
	};

	template<class T>
	using ComponentManager = TypedManager<T>;

	template<class T>
	using EntityManager = TypedManager<T>;
}
