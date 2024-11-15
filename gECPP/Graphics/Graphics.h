//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Prototype.h>
#include <Serializable/Serializable.h>

#define API_NONE 0
#define API_GL 1

#if API_ID == API_GL
	#define API GL
#elif API_ID == API_NONE
	#define API GPU
#endif

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
		explicit APIObject() = default;
		explicit APIObject(gE::Window* window) : _window(window) { };

		DELETE_OPERATOR_COPY(APIObject);
		OPERATOR_MOVE(APIObject, o, ID = o.ID; o.ID = 0);

		virtual void Bind() const = 0;

		GET_CONST(T,, ID);
		GET_CONST(gE::Window&, Window, *_window);

		virtual ~APIObject() = default;

	 protected:
		T ID {};

	 private:
		gE::Window* _window;
	};
}
