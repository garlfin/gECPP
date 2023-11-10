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
}

