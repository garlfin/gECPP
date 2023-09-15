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
	struct AssetHandle;

	struct AssetContainer
	{
		explicit AssetContainer(GL::Asset* t) : _t(t) {}

		AssetContainer(const AssetContainer&&) = delete;
		AssetContainer(const AssetContainer&) = delete;

		GET_CONST(u32, RefCount, _refCount);
		GET_BOTH(GL::Asset*, , _t);
		ALWAYS_INLINE void Inc() { ++_refCount; }
		ALWAYS_INLINE void Dec() { --_refCount; }

		~AssetContainer() { delete _t; }

	 private:
		GL::Asset* _t = nullptr;
		u32 _refCount = 0;
	};

	template<typename T, typename... ARGS>
	AssetContainer* CreateContainer(ARGS&&... args) { return new AssetContainer(new T(args...)); }

	template<class T>
	struct AssetHandle
	{
		inline AssetHandle(AssetContainer& container) : _container(&container) { _container->Inc(); }
		inline AssetHandle(const AssetHandle& h) : _container(h._container) { _container->Inc(); }
		inline AssetHandle() = default;

		ALWAYS_INLINE T* operator->() const { return (T*) _container->Get(); }

		AssetHandle& operator=(const AssetHandle& o)
		{
			if(&o == this) return *this;

			this->~AssetHandle();
			new(this) AssetHandle(o);

			return *this;
		}

		~AssetHandle() { if(_container) _container->Dec(); }

	 private:
		AssetContainer* _container = nullptr;
	};

 	class AssetManager : private std::vector<AssetContainer*>
	{
	 public:
		AssetManager() = default;

		template<class T, typename... ARGS>
		inline AssetHandle<T> Create(ARGS&&... args)
		{
			static_assert(std::is_base_of_v<GL::Asset, T>, "T should be an asset!");
			AssetContainer* container = CreateContainer<T>(args...);
			push_back(container);
			return AssetHandle<T>(*container);
		}

		template<class T>
		inline AssetHandle<T> Register(T* t)
		{
			push_back(new AssetContainer(t));
			return AssetHandle<T>(*back());
		}

		void Collect()
		{
			for(AssetContainer* t : *this)
				if (!t->GetRefCount())
				{
					Remove(t);
					delete t;
				}
		}


		NODISCARD ALWAYS_INLINE size_t Size() const { return size(); }

		~AssetManager()
		{
			for(auto* t : *this) delete t;
		}
	 private:
		// inline bool Contains(AssetContainer* v) { return std::find(begin(), end(), v) != end(); }
		virtual void Remove(AssetContainer* t)
		{
			auto f = std::find(begin(), end(), t);
			//if(f == end()) return;

			std::iter_swap(f, end() - 1);
			erase(end() - 1);
		}

	};
}
