//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>

#define API_NONE 0
#define API_GL 1

#if API_ID == API_GL
	#define API GL
#endif

#if API_ID != API_NONE
	#define ASSET_CHECK_FREE() if(!IsFree()) LOG("WARNING: ASSET NOT FREED BEFORE DELETION!")
#else
	#define ASSET_CHECK_FREE()
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
		APIObject() = default;
		explicit APIObject(gE::Window* window) : _window(window) { };

		DELETE_COPY_CONSTRUCTOR(APIObject);
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

	class Asset
	{
		DEFAULT_CM_CONSTRUCTOR(Asset);

	public:
		Asset() = default;

		virtual void Free() = 0;
		NODISCARD virtual bool IsFree() const = 0;

		virtual ~Asset() = default;
	};
}
