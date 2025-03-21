//
// Created by scion on 8/16/2023.
//

#pragma once

#include <fstream>
#include <Prototype.h>
#include <unordered_map>
#include <Core/Array.h>
#include <Core/Binary.h>
#include <Core/Macro.h>
#include <Core/Math/Math.h>

#include "Reflectable.h"

struct Underlying
{
	virtual ~Underlying() = default;
};

struct ISerializable
{
public:
	ISerializable() = default;

	virtual Underlying* GetUnderlying() { return nullptr; }
	virtual const Underlying* GetUnderlying() const { return nullptr; }

	template<class I>
	NODISCARD bool IsCastable() const
	{
		const Underlying* underlying = GetUnderlying();

		return dynamic_cast<const I*>(this) || dynamic_cast<const I*>(underlying);
	}

	virtual ~ISerializable() = default;
};

template<class T>
struct Serializable : public Reflectable<T>, public ISerializable
{
	DEFAULT_OPERATOR_CM(Serializable);
	REFLECTABLE_NOIMPL(Reflectable<T>);

public:
	Serializable() = default;
	Serializable(istream&, T) { }

	using SETTINGS_T = T;

	virtual void Deserialize(istream& in, SETTINGS_T settings) {};
	virtual void Serialize(ostream& out) const {};
	virtual void Reload(SETTINGS_T settings) {};

	using ISerializable::IsCastable;

	~Serializable() override = default;
};

#define SERIALIZABLE_CHECK_HEADER() \
	char magic[4]; \
	Read<char>(in, 4, magic); \
	GE_ASSERTM(strcmpb(magic, MAGIC, 4), "UNEXPECTED MAGIC!"); \
	_version = Read<u8>(in);

#define SERIALIZABLE_PROTO(NAME, MAGIC_VAL, VERSION_VAL, TYPE, SUPER_T, ...) \
	public: \
		typedef SUPER_T SUPER; \
		typedef SUPER::SETTINGS_T SETTINGS_T;\
		TYPE(istream& in, SETTINGS_T s) : SUPER(in, s) { SERIALIZABLE_CHECK_HEADER(); IDeserialize(in, s); } \
		TYPE() = default; \
		REFLECTABLE_MAGIC_IMPL(MAGIC_VAL); \
		inline void Deserialize(istream& in, SETTINGS_T s) override { PlacementNew(*this, in, s); } \
		inline void Serialize(ostream& out) const override { SUPER::Serialize(out); Write(out, 4, MAGIC); Write<u8>(out, _version); ISerialize(out); } \
		GET_CONST(u8, Version, _version) \
		DEFAULT_OPERATOR_CM(TYPE); \
	private: \
		void IDeserialize(istream& in, SETTINGS_T s); \
		void ISerialize(ostream& out) const; \
		u8 _version = VERSION_VAL; \
		REFLECTABLE_PROTO(TYPE, SUPER_T, NAME, __VA_ARGS__)

#define SERIALIZABLE_PROTO_NOHEADER(NAME, TYPE, SUPER_T, ...) \
	public: \
		typedef SUPER_T SUPER; \
		typedef SUPER::SETTINGS_T SETTINGS_T;\
		TYPE(istream& in, SETTINGS_T s) : SUPER(in, s) { IDeserialize(in, s); } \
		TYPE() = default; \
		REFLECTABLE_MAGIC_IMPL(""); \
		inline void Deserialize(istream& in, SETTINGS_T s) override { PlacementNew(*this, in, s); } \
		inline void Serialize(ostream& out) const override { SUPER::Serialize(out); ISerialize(out); } \
		GET_CONST(u8, Version, 0) \
		DEFAULT_OPERATOR_CM(TYPE); \
	private: \
		void IDeserialize(istream& in, SETTINGS_T s); \
		void ISerialize(ostream& out) const; \
		u8 _version = 0; \
		REFLECTABLE_PROTO(TYPE, SUPER_T, NAME, __VA_ARGS__)

template<class T, class S>
concept is_serializable_in = requires(T t, S s, std::istream& i)
{
	t.Deserialize(i, s);
};

template<class T>
concept is_serializable_out = requires(T t, std::ostream& o)
{
	t.Serialize(o);
};

template<class T, class S>
concept is_serializable = is_serializable_in<S, T> and is_serializable_out<T>;

template<class T> struct Serializable;
template<is_serializable<gE::Window*> T> struct FileContainer;

template<is_serializable_in<gE::Window*> T> void ReadSerializableFromFile(gE::Window*, const Path&, T&);
template<is_serializable_in<gE::Window*> T> T* ReadSerializableFromFile(gE::Window*, const Path&);
template<is_serializable_out T> void WriteSerializableToFile(const Path&, const T&);

template<class T> void Read(std::istream& in, u32 count, T* t);
template<class T> void Read(std::istream& in, T& t) { Read(in, 1, &t); }
template<class T> T Read(std::istream& in) { T t; Read(in, t); return t; }

template<class T> void Write(std::ostream& out, u32 count, const T* t);
template<class T> void Write(std::ostream& out, const T& t) { Write(out, 1, &t); }

template<typename UINT_T, class T> void Read(std::istream& in, u32 count, Array<T>* t);
template<typename UINT_T, class T> void Read(std::istream& in, Array<T>& t) { Read<UINT_T>(in, 1, &t); }

template<typename UINT_T, class T> void WriteArray(std::ostream& out, u32 count, const Array<T>* t);
template<typename UINT_T, class T> void WriteArray(std::ostream& out, const Array<T>& t) { WriteArray<UINT_T>(out, 1, &t); }

template<typename UINT_T, class T> void ReadArraySerializable(std::istream& in, u32 count, Array<T>* t, typename T::SETTINGS_T s);
template<typename UINT_T, class T> void ReadArraySerializable(std::istream& in, Array<T>& t, typename T::SETTINGS_T s) { ReadArraySerializable<UINT_T>(in, 1, &t, s); }

template<class T> void ReadSerializable(std::istream& in, u32 count, T* t, typename T::SETTINGS_T s);
template<class T> void ReadSerializable(std::istream& in, T& t, typename T::SETTINGS_T s) { ReadSerializable<T>(in, 1, &t, s); }

template<class T>
const Type<T>* ReadType(std::istream& in)
{
	return TypeSystem<T>::GetTypeInfo(Read<std::string>(in));
}

template<class T>
void WriteType(std::ostream& out, const Type<T>& type)
{
	Write(out, type.Name);
}

template<> void Read(std::istream& in, u32 count, std::string* t);
template<> void Write(std::ostream& out, u32 count, const std::string* t);

template<> void Write(std::ostream& out, u32 count, const std::string_view* t);

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

		UINT_T length = arr.Count();

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
	for(u32 i = 0; i < count; i++) t[i] = T(in, s);
}