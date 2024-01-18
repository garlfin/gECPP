//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>

#define SERIALIZABLE_PROTO  void Deserialize(gETF::SerializationBuffer& buf, const SETTINGS_T& settings) const override;\
							void Serialize(u8*& ptr, const SETTINGS_T& settings) override

namespace gETF
{
	struct SerializationBuffer;

	template<class T>
	struct Serializable
	{
		typedef T SETTINGS_T;

		virtual void Deserialize(SerializationBuffer& buf, const T& s) const = 0;
		virtual void Serialize(u8*& ptr, const T& s) = 0;
	};

	template<typename T, typename S> T* ReadNewSerializable(u8*& src, const S& s)
	{
		static_assert(std::is_default_constructible_v<T>);
		T* t = new T();
		t->Serialize(src, s);
		return t;
	}

	template<typename T, typename S> void ReadSerializable(u8*& src, T* ts, const S& s, u32 count)
	{
		for(u32 i = 0; i < count; i++)
			ts[i].Serialize(src, s);
	}

	template<u32 COUNT, typename T, typename S> void ReadSerializable(u8*& src, T* ts, const S& s)
	{
		for(u32 i = 0; i < COUNT; i++)
			ts[i].Serialize(src, s);
	}
}
