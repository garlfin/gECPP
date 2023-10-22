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

	template<class T>
 	class Manager : private std::vector<T*>
	{
	 public:
		Manager() = default;

		inline virtual void Register(T* t) { if(!Contains(t)) vec::push_back(t); }
		inline virtual void Remove(T* t)
		{
			 auto f = std::find(vec::begin(), vec::end(), t);
			 if(f == vec::end()) return;

			 std::iter_swap(f, vec::end() - 1);
			 vec::erase(vec::end() - 1);
		}

		virtual void OnUpdate(float delta)
		{
			for(T* t : *this)
				if(t->GetFlags().Deletion)
					t->OnDestroy();
				else
					t->OnUpdate(delta);
		}
		virtual void OnRender(float delta)
		{
			for(T* t : *this)
				t->OnRender(delta);
		}

		using std::vector<T*>::operator[];

		NODISCARD ALWAYS_INLINE size_t Size() const { return vec::size(); }

	 private:
		typedef std::vector<T*> vec;
		inline bool Contains(T* v) { return std::find(vec::begin(), vec::end(), v) != vec::end(); }
	};

	template<class T>
	using ComponentManager = Manager<T>;
}

