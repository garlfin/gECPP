//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>

#define SERIALIZABLE_PROTO  void Deserialize(gETF::SerializationBuffer& buf) const override;\
							void Serialize(u8*& ptr) override

namespace gETF
{
	struct SerializationBuffer;

	struct Serializable
	{
		virtual void Deserialize(SerializationBuffer& buf) const = 0;
		virtual void Serialize(u8*& ptr) = 0;
	};

	template<class T>
	T* ReadNew(u8*& ptr)
	{
		T* t = new T();
		t->Serialize(ptr);
		return t;
	}
}
