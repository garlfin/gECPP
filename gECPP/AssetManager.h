//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include <GL/gl.h>

namespace gE
{
	class Window;

	class AssetManager : private std::vector<GL::Asset*>
	{
	 public:
		AssetManager(Window* w) : _window(w) {}

		template<class T, typename... ARGS>
		inline T* Register(ARGS&&... args) { vec::emplace_back(std::forward(args...)); }
		inline virtual GL::Asset* Register(GL::Asset* t) { if(!Contains(t)) vec::push_back(t); return t; }
		inline virtual void Remove(GL::Asset* t)
		{
			auto f = std::find(vec::begin(), vec::end(), t);
			if(f == vec::end()) return;

			std::iter_swap(f, vec::end() - 1);
			vec::erase(vec::end() - 1);
		}

		~AssetManager()
		{
			for(auto* t : *this) delete t;
			vec::~vec();
		}
	 private:
		typedef std::vector<GL::Asset*> vec;
		inline bool Contains(GL::Asset* v) { return std::find(vec::begin(), vec::end(), v) != vec::end(); }
		Window* const _window;
	};
}
