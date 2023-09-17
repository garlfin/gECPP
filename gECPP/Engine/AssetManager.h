//
// Created by scion on 9/7/2023.
//

#pragma once

#include <vector>
#include "GL/gl.h"

namespace gE
{
	class Window;

	template<class T>
	struct AssetHandle
	{
		static_assert(std::is_base_of_v<GL::Asset, T>, "T SHOULD BE AN ASSET!");
		inline AssetHandle(T* t) : _t(t), _counter(new u32(1)) {}
		inline AssetHandle(const AssetHandle& h) : _t(h._t), _counter(h._counter) { ++(*_counter); }
		inline AssetHandle() = default;

		ALWAYS_INLINE T* operator->() const { return _t; }

		template<typename... ARGS>
		inline static AssetHandle Create(ARGS&&... args) { return AssetHandle(new T(args...)); }

		AssetHandle& operator=(const AssetHandle& o)
		{
			if(&o == this) return *this;

			this->~AssetHandle();
			new(this) AssetHandle(o);

			return *this;
		}

		~AssetHandle()
		{
			if(!_t || --(*_counter)) return;

			std::cout << "FREE!" << std::endl;
			delete _t;
			delete _counter;
		}

	 private:
		T* _t = nullptr;
		u32* _counter = nullptr;
	};

	template<class T, typename... ARGS>
	AssetHandle<T> CreateHandle(ARGS&&... args)
	{
		return AssetHandle<T>::Create(std::forward<ARGS>(args)...);
	}

	template<class T>
	AssetHandle<T> CreateHandleFromPointer(T* t)
	{
		return AssetHandle<T>(t);
	}
}
