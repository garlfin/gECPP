//
// Created by scion on 4/30/2025.
//

#pragma once

#include "Serializable.h"

template <class T>
void Serializable<T>::Deserialize(istream& in, SETTINGS_T settings)
{
	//Read(in, _uuid);
}

template <class T>
void Serializable<T>::Serialize(ostream& out) const
{
	Write(out, _UUID);
}

template<is_serializable_in<gE::Window*> T>
void ReadSerializableFromFile(gE::Window* window, const Path& path, T& t)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	GE_ASSERTM(file.is_open(), "COULD NOT OPEN FILE!");

	t.Deserialize(file, window);

	file.close();
}

template<is_serializable_in<gE::Window*> T>
T* ReadSerializableFromFile(gE::Window* window, const Path& path)
{
	T* t = new T();
	ReadSerializableFromFile(window, path, *t);
	return t;
}

template<is_serializable_out T>
void WriteSerializableToFile(const Path& path, const T& t)
{
	std::ofstream file(path, std::ios::out | std::ios::binary);
	t.Serialize(file);
}

template<typename UINT_T, class T>
void Read(std::istream& in, u32 count, Array<T>* t)
{
	for(u32 i = 0; i < count; i++)
	{
		Array<T>& arr = t[i];

		UINT_T length = Read<UINT_T>(in);

		arr = Array<T>(length);
		Read<T>(in, length, arr.Data());
	}
}

template<typename UINT_T, class T>
void WriteArray(std::ostream& out, u32 count, const Array<T>* t)
{
	for(u32 i = 0; i < count; i++)
	{
		const Array<T>& arr = t[i];

		UINT_T length = arr.Size();

		Write(out, length);
		Write(out, length, arr.Data());
	}
}

template<typename UINT_T, class T>
void ReadArray(std::istream& in, Array<T>& array)
{
	static_assert(std::is_trivially_copyable_v<T>);

	UINT_T length = Read<UINT_T>(in);
	array = Array<T>(length);
	in.read((char*) array.Data(), length * sizeof(typename Array<T>::I));
}

template<typename UINT_T, class T>
Array<T> ReadArray(std::istream& in)
{
	Array<T> array;
	ReadArray<UINT_T, T>(in, array);
	return array;
}

template<>
inline void Read(std::istream& in, u32 count, std::string* t)
{
	for(u32 i = 0; i < count; i++)
	{
		std::string& str = t[i];

		u32 length = Read<u32>(in);

		str = std::string(length, 0);
		Read(in, length, str.data());
	}
}


template<>
inline void Write(std::ostream& out, u32 count, const std::string* t)
{
	for(u32 i = 0; i < count; i++)
	{
		const std::string& str = t[i];

		u32 length = str.length();

		Write(out, length);
		Write(out, length, str.c_str());
	}
}

template<>
inline void Write(std::ostream& out, u32 count, const std::string_view* t)
{
	for(u32 i = 0; i < count; i++)
	{
		const std::string_view& str = t[i];

		u32 length = str.length();

		Write(out, length);
		Write(out, length, str.data());
	}
}

template<typename T>
void Read(istream& in, u32 count, T* t)
{
	if(!count) return;
	static_assert(std::is_trivially_copyable_v<T> && !is_serializable_out<T>);
	in.read((char*) t, sizeof(T) * count);
}

template<typename T>
void Write(ostream& out, u32 count, const T* t)
{
	if(!count) return;
	if constexpr(std::is_trivially_copyable_v<T> && !is_serializable_out<T>)
		out.write((char*) t, sizeof(T) * count);
	else
		for(u32 i = 0; i < count; i++) t[i].Serialize(out);
}

template<typename UINT_T, class T>
void ReadArraySerializable(std::istream& in, u32 count, Array<T>* ts, typename T::SETTINGS_T s)
{
	for(u32 i = 0; i < count; i++)
	{
		Array<T>& t = ts[i];

		UINT_T length = Read<UINT_T>(in);

		t = Array<T>(length);
		ReadSerializable(in, length, t.Data(), s);
	}
}

template<class T>
void ReadSerializable(std::istream& in, u32 count, T* t, typename T::SETTINGS_T s)
{
	if(!count) return;
	for(u32 i = 0; i < count; i++) PlacementNew<T>(t[i], in, s);
}