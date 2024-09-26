//
// Created by scion on 9/7/2023.
//

#pragma once

#include <gECPP/Prototype.h>
#include <gECPP/Engine/Utility/Macro.h>

#include "RelativePointer.h"

namespace gE
{
	template<class T>
	using CompareFunc = bool(*)(T a, T b);

	enum class SearchDirection : u8
	{
		Left,
		Right
	};

	template<class T>
	class ManagedList
	{
	 public:
		ManagedList() = default;

		DELETE_CM_CONSTRUCTOR(ManagedList);

		void Add(Managed<T>& t);
		void Insert(Managed<T>& t, Managed<T>& at);
		void Move(Managed<T>& t, Managed<T>& to);
		void Remove(Managed<T>& t);
		void MergeList(ManagedList& list);

		template<CompareFunc<const T&> COMPARE_FUNC>
		ALWAYS_INLINE Managed<T>* FindSimilar(Managed<T>& similar);

		template<CompareFunc<const T&> COMPARE_FUNC>
		Managed<T>* FindSimilar(Managed<T>& similar, Managed<T>& start, SearchDirection direction);

		GET_CONST(Managed<T>*, First, _first);
		GET_CONST(Managed<T>*, Last, _last);
		GET_CONST(u32, Size, _size);

	 private:
		Managed<T>* _first = nullptr;
		Managed<T>* _last = nullptr;
		u32 _size = 0;
	};

	template<class T>
	class Manager
	{
	 public:
		virtual ~Manager() = default;

		Manager() = default;

		DELETE_CM_CONSTRUCTOR(Manager);

		virtual void OnUpdate(float d) = 0;
		virtual void OnRender(float d, Camera* camera) = 0;

		friend class Managed<T>;

	 protected:
		virtual void Register(T& t)
		{
			List.Add(t);
		}

		virtual void Remove(T& t)
		{
			List.Remove(t);
		}

		ManagedList<T> List;
	};

	template<class T>
	class Managed
	{
	 public:
		explicit inline Managed(T& t, Manager<T>* m = nullptr) : _manager(m), _t(&t)
		{
			if(m) m->Register(t);
		}

		OPERATOR_COPY(Managed, o,
			_manager = o._manager;
			_t = o._t;
			_next = _previous = nullptr;
			if(_manager) _manager->Register(*_t);
		);

		OPERATOR_MOVE(Managed, o,
			_manager = o._manager;
			_t = o._t;
			_next = _previous = nullptr;
			if(_manager)
			{
				_manager->Register(*_t);
				_manager->Remove(*o._t);
			}
		);

		GET_CONST(Managed*, Previous, _previous);
		GET_CONST(Managed*, Next, _next);
		GET_CONST(ManagedList<T>*, List, _list);
		GET_CONST(Manager<T>*, Manager, _manager);
		GET_CONST(T&,, *_t);

		NODISCARD ALWAYS_INLINE T* operator->() { return (T*) _t; }
		NODISCARD ALWAYS_INLINE const T* operator->() const { return (T*) _t; }

		virtual inline ~Managed() { if(_manager) _manager->Remove(*_t); }

	 private:
		ManagedList<T>* _list = nullptr;
		Manager<T>* _manager;

		Managed* _previous = nullptr;
		RelativePointer<T> _t;
		Managed* _next = nullptr;

		friend class ManagedList<T>;
	};
}

#include "Manager.inl"