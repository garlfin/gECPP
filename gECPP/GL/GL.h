#pragma once

#include "Math.h"

#ifndef NODISCARD
	#define NODISCARD [[nodiscard]]
#endif
#ifndef ALWAYS_INLINE
	#ifdef DEBUG
		#define ALWAYS_INLINE inline
	#else
		#define ALWAYS_INLINE __attribute__((always_inline)) inline
	#endif // #if DEBUG
#endif // #ifndef ALWAYS_INLINE

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