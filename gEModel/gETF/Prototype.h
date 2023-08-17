//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GLMath.h>

#define SERIALIZABLE_PROTO  void Deserialize(SerializationBuffer& buf) const override;\
							void Serialize(u8*& ptr) override

namespace gETF
{
	struct SerializationBuffer;

	struct Serializable
	{
		virtual void Deserialize(SerializationBuffer& buf) const = 0;
		virtual void Serialize(u8*& ptr) = 0;
	};
}
