//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>

namespace GPU
{
	struct Version
	{
		u8 Major;
		u8 Minor;
	};

	template<class T>
	class APIObject
	{
	 public:
		explicit APIObject(gE::Window* window) : _window(window) { };
		APIObject(APIObject&) = delete;

		APIObject& operator=(const APIObject&) = delete;

		OPERATOR_EQUALS_XVAL(APIObject, o, _window = o._window; ID = o.ID; o.ID = 0);

		virtual void Bind() const = 0;

		GET_CONST(T,, ID);
		GET_CONST(gE::Window&, Window, *_window);

		virtual ~APIObject() = default;

	 protected:
		T ID;

	 private:
		gE::Window* _window;
	};

	struct Asset
	{
	 public:
		Asset() = default;

		virtual void Free() { _isFree = true; }
		NODISCARD ALWAYS_INLINE bool IsFree() const { return _isFree; }

		virtual ~Asset() { _isFree = true; }

	 private:
		bool _isFree = true;
	};
}
