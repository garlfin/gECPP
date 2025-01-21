#pragma once

#include <Prototype.h>
#include <Serializable/Serializable.h>

#define GETF_VERSION 1

namespace gE
{
	struct Header : public Serializable<const Header&>
	{
	 public:
		SERIALIZABLE_PROTO("HDR", GETF_VERSION, Header, Serializable<const Header&>);

		gE::Window* Window;

	 private:

		Array<u8> Files;
	};
  }
