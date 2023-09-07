//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include <Components/Component.h>

namespace gE
{
	class Window;

 	class ComponentManager : private std::vector<Component*>
	{
	 public:
		explicit ComponentManager(Window* w) : _window(w) {}

		inline virtual void Register(Component* t) { if(!Contains(t)) vec::push_back(t); }
		inline virtual void Remove(Component* t)
		{
			 auto f = std::find(vec::begin(), vec::end(), t);
			 if(f == vec::end()) return;

			 std::iter_swap(f, vec::end() - 1);
			 vec::erase(vec::end() - 1);
		}
	 private:
		typedef std::vector<Component*> vec;
		inline bool Contains(Component* v) { return std::find(vec::begin(), vec::end(), v) != vec::end(); }
		Window* const _window;
	};
}