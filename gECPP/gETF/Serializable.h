//
// Created by scion on 8/16/2023.
//

#pragma once

#include <GL/Math.h>
#include <Engine/Array.h>
#include <GL/Binary/Binary.h>

using std::istream;
using std::ostream;

#define SERIALIZABLE_PROTO_T void Deserialize(ostream& buf, const SETTINGS_T& settings) const override;\
							 void Serialize(istream& ptr, const SETTINGS_T& settings) override

#define SERIALIZABLE_PROTO void Deserialize(ostream& buf) const override;\
						   void Serialize(istream& ptr) override


template<class T = void>
struct Serializable
{
	typedef T SETTINGS_T;

	virtual void Deserialize(ostream& buf, const T& s) const = 0;
	virtual void Serialize(istream& ptr, const T& s) = 0;

	virtual ~Serializable() = default;
};

template<>
struct Serializable<void>
{
	virtual void Deserialize(ostream& buf) const = 0;
	virtual void Serialize(istream& ptr) = 0;

	virtual ~Serializable() = default;
};

template<typename T, typename S>
T* ReadNewSerializable(istream& src, const S& s)
{
	static_assert(std::is_default_constructible_v<T>);
	T* t = new T();
	t->Serialize(src, s);
	return t;
}

template<typename T, typename S>
void ReadSerializable(istream& src, T* ts, const S& s, u32 count)
{
	for(u32 i = 0; i < count; i++)
		ts[i].Serialize(src, s);
}

template<typename T, typename S>
void WriteSerializable(ostream& src, T* ts, const S& s, u32 count)
{
	for(u32 i = 0; i < count; i++)
		ts[i].Deserialize(src, s);
}

template<typename UINT_T, class T, class S>
Array<T> ReadArraySerializable(istream& src, const S& s)
{
	UINT_T count = ::Read<UINT_T>(src);

	Array<T> arr(count);
	ReadSerializable(src, arr.Data(), s, count);
	return arr;
}

template<typename UINT_T, class T, class S>
void WriteArraySerializable(ostream& dst, const Array<T>& src, const S& s)
{
	Write<UINT_T>(dst, src.Count());
	WriteSerializable(dst, src.Data(), s, src.Count());
}
