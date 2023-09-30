#pragma once

#include "Math.h"
#include "GL/Binary.h"

namespace gE { class Window; }

namespace GL
{
	struct Version
	{
		ubyte Major;
		ubyte Minor;
	};

	struct Asset
	{
	 public:
		Asset(Asset&&) = delete;
		Asset(Asset&) = delete;

		Asset& operator=(const Asset&) = delete;
		Asset& operator=(Asset&&) = delete;

		NODISCARD ALWAYS_INLINE uint32_t Get() const { return ID; }
		virtual void Bind() const = 0;
		NODISCARD ALWAYS_INLINE gE::Window* GetWindow() const { return _window; }

		virtual ~Asset() = default;

	 protected:
		Asset(gE::Window* window) : _window(window) { };
		uint32_t ID;

	 private:
		gE::Window *const _window;
	};
}