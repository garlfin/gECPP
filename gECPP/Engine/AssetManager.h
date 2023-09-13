//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/GL.h"

namespace gE
{
	class Window;

	class AssetManager : private std::vector<GL::Asset*>
	{
	 public:
		AssetManager() = default;

		template<class T, typename... ARGS>
		inline T* Create(ARGS&&... args)
		{
			static_assert(std::is_base_of_v<GL::Asset, T>, "T should be an asset!");
			T* t = new T(args...);
			push_back(t);
			return t;
		}

		template<class T>
		inline T* Register(T* t) { if(!Contains(t)) push_back(t); return t; }
		inline virtual void Remove(GL::Asset* t)
		{
			auto f = std::find(begin(), end(), t);
			if(f == end()) return;

			std::iter_swap(f, end() - 1);
			erase(end() - 1);
		}

		using std::vector<GL::Asset*>::operator[];

		NODISCARD ALWAYS_INLINE size_t Size() const { return size(); }

		~AssetManager()
		{
			for(auto* t : *this) delete t;
		}
	 private:
		inline bool Contains(GL::Asset* v) { return std::find(begin(), end(), v) != end(); }
	};
}
