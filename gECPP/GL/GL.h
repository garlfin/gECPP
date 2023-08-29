#pragma once

#include "GLMath.h"

#define NODISCARD [[nodiscard]]
#define ALWAYS_INLINE __attribute__((always_inline))

#define GE_FAIL(ERR) { std::cout << ERR << std::endl; exit(-1); }
#define GE_ASSERT(COND, ERR) if(!(COND)) GE_FAIL(ERR);

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
	 protected:
		Asset(gE::Window* window) : Window(window) { };
		gE::Window *const Window;
		uint32_t ID;

	 public:
		NODISCARD ALWAYS_INLINE uint32_t Get() const { return ID; }
		virtual void Bind() const = 0;
		NODISCARD ALWAYS_INLINE gE::Window* GetWindow() const { return Window; }
		virtual ~Asset() = default;
	};
}