#pragma once

#include "Math.h"
#include "GL/Binary/Binary.h"

namespace gE
{
	class Window;
}

namespace GL
{
	struct Version
	{
		u8 Major;
		u8 Minor;
	};

	struct Asset
	{
	 public:
		Asset(Asset&&) = delete;
		Asset(Asset&) = delete;

		Asset& operator=(const Asset&) = delete;
		Asset& operator=(Asset&&) = delete;

		virtual void Bind() const = 0;

		GET_CONST(u32, , ID);
		GET_CONST(gE::Window&, Window, *_window);

		virtual ~Asset() = default;

	 protected:
		Asset(gE::Window* window) : _window(window) { };
		uint32_t ID;

	 private:
		gE::Window* _window;
	};
}