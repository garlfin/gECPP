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