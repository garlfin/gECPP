//
// Created by scion on 8/16/2023.
//

#pragma once

#include <Prototype.h>
#include <GL/Math.h>
#include <Engine/Array.h>
#include <GL/Binary/Binary.h>
#include <fstream>

using std::istream;
using std::ostream;

#define SERIALIZABLE_PROTO_T void Deserialize(ostream& buf) const override;\
							 void Serialize(istream& ptr, const SETTINGS_T& settings) override

#define SERIALIZABLE_PROTO void Deserialize(ostream& buf) const override;\
						   void Serialize(istream& ptr) override

template<class T = void>
struct Serializable
{
	typedef T SETTINGS_T;

	virtual void Deserialize(ostream& buf) const = 0;
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
T* ReadSerializable(istream& src, const S& s)
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

template<typename T>
void WriteSerializable(ostream& src, T* ts, u32 count)
{
	for(u32 i = 0; i < count; i++)
		ts[i].Deserialize(src);
}

template<typename UINT_T, class T, class S>
Array<T> ReadArraySerializable(istream& src, const S& s)
{
	UINT_T count = ::Read<UINT_T>(src);

	Array<T> arr(count);
	ReadSerializable(src, arr.Data(), s, count);
	return arr;
}

template<typename UINT_T, class T>
void WriteArraySerializable(ostream& dst, const Array<T>& src)
{
	Write<UINT_T>(dst, src.Count());
	WriteSerializable(dst, src.Data(), src.Count());
}

struct TypeID
{
	template<class T>
	constexpr explicit TypeID(const char* name) : Type(0), Size(sizeof(T))
	{
		u32 len = strlen(name);
		for(u32 i = 0; i < len; i++)
			Type ^= name[i] >> i % 8;
	}

	TypeID() = default;

	u64 Type = 0;
	u64 Size = 0;
};

namespace gETF
{
 	struct IFileContainer : public Serializable<gE::Window*>
	{
	 public:
		SERIALIZABLE_PROTO_T;

		template<class T> requires requires { T::Type; }
		explicit IFileContainer(T& t, const std::string& path) : _name(path), _type(T::Type), _t(&t)
		{
			static_assert(std::is_base_of_v<IFileContainer, T>);
		}

		template<class T> requires requires { T::Type; }
		T* Cast()
		{
			static_assert(std::is_base_of_v<IFileContainer, T>);
			if(T::Type == _type)
				return (T*) this;
			return nullptr;
		}

		template<class T>
		T& Cast() { T* t = Cast<T>(); assert(t); return *t; }

		GET_CONST(const std::string&, Name, _name);
		GET_CONST(TypeID, Type, _type);
		GET(Serializable<gE::Window*>&,, _t);

	 private:
		std::string _name;
		TypeID _type;
		Serializable<gE::Window*>& _t;
	};

	template<class T> requires std::is_base_of_v<Serializable<gE::Window*>, T>
	struct FileContainer : public IFileContainer
	{
		template<typename ... ARGS>
		explicit FileContainer(std::string& path, ARGS&&... args) :
			IFileContainer(Object, path),
			Object(std::forward(args)...)
		{};

		explicit FileContainer(std::string& path) : IFileContainer(Object, path) {};

		T Object;
	};
}

template<class T> requires std::is_base_of_v<Serializable<gE::Window*>, T>
void ReadSerializableFromFile(gE::Window* w, const char* path, T& t)
{
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	assert(file.is_open());

	t.Serialize(file, w);

	file.close();
}

template<class T> requires std::is_base_of_v<Serializable<gE::Window*>, T>
T* ReadSerializableFromFile(gE::Window* w, const char* path)
{
	T* t = new T();
	ReadSerializableFromFile(w, path, *t);
	return t;
}

template<class T> requires std::is_base_of_v<Serializable<gE::Window*>, T>
void WriteSerializableToFile(const char* path, const T& t)
{
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::binary);

	t.Deserialize(file);

	file.close();
}
