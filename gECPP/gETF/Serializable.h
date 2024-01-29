//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>
#include <Engine/Array.h>

#define SERIALIZABLE_PROTO  void Deserialize(SerializationBuffer& buf, const SETTINGS_T& settings) const override;\
							void Serialize(u8*& ptr, const SETTINGS_T& settings) override

struct SerializationBuffer;

template<class T>
struct Serializable
{
	typedef T SETTINGS_T;

	virtual void Deserialize(SerializationBuffer& buf, const T& s) const = 0;
	virtual void Serialize(u8*& ptr, const T& s) = 0;
};

template<typename T, typename S>
T* ReadNewSerializable(u8*& src, const S& s)
{
	static_assert(std::is_default_constructible_v<T>);
	T* t = new T();
	t->Serialize(src, s);
	return t;
}

template<typename T, typename S>
void ReadSerializable(u8*& src, T* ts, const S& s, u32 count)
{
	for(u32 i = 0; i < count; i++)
		ts[i].Serialize(src, s);
}

template<u32 COUNT, typename T>
void ReadSerializable(u8*& src, Serializable<T>* ts, const T& s)
{
	for(u32 i = 0; i < COUNT; i++)
		ts[i].Serialize(src, s);
}

template<typename UINT_T, class T, class S>
Array<T> ReadArraySerializable(u8*& src, const S& s)
{
	UINT_T count = *(UINT_T*) src;
	src += sizeof(UINT_T);

	Array<T> arr(count);
	ReadSerializable(src, arr.Data(), s, count);
	return arr;
}
