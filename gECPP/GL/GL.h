#pragma once

#include "Math.h"
#include "Engine/Binary/Binary.h"
#include <gETF/Serializable.h>

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

 	struct Asset : public Serializable<gE::Window*>
	{
		SERIALIZABLE_PROTO_T(Asset, Serializable<gE::Window*>);

	 public:
		Asset(gE::Window* window) : _window(window) { };
		Asset(Asset&) = delete;

		Asset& operator=(const Asset&) = delete;

		OPERATOR_EQUALS_XVAL(Asset, o, ID = o.ID; o.ID = 0);

		virtual void Bind() const = 0;

		// Releases any CPU-side data.
		virtual void Free() {};

		GET_CONST(u32, , ID);
		GET_CONST(gE::Window&, Window, *_window);

		~Asset() override = default;

	 protected:
		uint32_t ID;

	 private:
		gE::Window* _window;
	};

	inline void Asset::ISerialize(std::istream& in, gE::Window* s) { _window = s; }
	inline void Asset::IDeserialize(std::ostream& out) const { }
}