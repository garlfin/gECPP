#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>

#define GETF_VERSION 1

namespace gETF
{
	struct Header : public Serializable<const Header&>
	{
	 public:
		SERIALIZABLE_PROTO(HDR, 1, Header, Serializable<const Header&>);

		gE::Window* Window;
		u8 Version = GETF_VERSION;

	 private:

		Array<u8> Files;
	};
  }
