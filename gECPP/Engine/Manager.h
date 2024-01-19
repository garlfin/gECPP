//
// Created by scion on 9/7/2023.
//

#pragma once

#include <Prototype.h>
#include <vector>
#include <algorithm>

namespace gE
{
	class Window;

	template<class T>
	class Manager : protected std::vector<T*>
	{
	 public:
		typedef std::vector<T*> VEC_T;

		Manager() = default;

		Manager(const Manager&) = delete;
		Manager(Manager&&) noexcept = delete;
		Manager<T>& operator=(const Manager&) = delete;
		Manager<T>& operator=(Manager&&) noexcept = delete;

		virtual void OnUpdate(float d) = 0;
		virtual void OnRender(float d, Camera* camera) = 0;

		NODISCARD ALWAYS_INLINE size_t Size() const { return VEC_T::size(); }

		using std::vector<T*>::operator[];

		friend class Managed<T>;

	 protected:
		inline void Register(T* t) { VEC_T::push_back(t); }
		inline void Remove(T* t) { RemoveFirstFromVec(*this, t); }

	 protected:
		inline bool Contains(T* v) { return std::find(VEC_T::begin(), VEC_T::end(), v) != VEC_T::end(); }
	};

	template<class T>
	class Managed
	{
	 public:
		explicit inline Managed(T& t, Manager<T>* m = nullptr) : _t(t), _manager(m) { if(_manager) _manager->Register(&t); }
		inline Managed(T& t, Manager<T>& m) : Managed(t, &m) {};

		inline ~Managed() { if(_manager) _manager->Remove(&_t); }

	 private:
		T& _t;
		Manager<T>* _manager;
	};
}