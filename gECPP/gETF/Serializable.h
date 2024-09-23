//
// Created by scion on 8/16/2023.
//

#pragma once

#include <fstream>
#include <Prototype.h>
#include <unordered_map>
#include <Engine/Utility/Array.h>
#include <Engine/Utility/Binary.h>
#include "Engine/Math/Math.h"

#include "Serializable.h"

// Defines SERIALIZABLE_PROTO

// TODO: REDO THIS AGAIN

template<class T>
struct TypeSystem
{
	using TypeID = u64;
	using FactoryFunction = std::conditional_t<std::is_same_v<T, void>,
		Serializable<T>*(*)(std::istream&),
		Serializable<T>*(*)(std::istream&, T)>;

	struct Type
	{
		Type(const char* n, FactoryFunction f) : Name(n), ID(0), Factory(f)
		{
			Types[ID];
		};

		const char* Name;
		TypeID ID;

		FactoryFunction Factory;
	};

	static GLOBAL std::unordered_map<TypeID, Type> Types;
};

template<class T, class S>
concept is_serializable_in = requires(T t, S s, std::istream& i)
{
	t.Serialize(i, s);
};

template<class T>
concept is_serializable_out = requires(T t, std::ostream& o)
{
	t.Deserialize(o);
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

template<> void Read(std::istream& in, u32 count, std::string* t);
template<> void Write(std::ostream& out, u32 count, const std::string* t);

#define SERIALIZABLE_MAGIC_LENGTH 4

#define VIRTUAL_H(FUNC_RETURN, FUNC) \
	inline FUNC_RETURN FUNC override {};

#define VIRTUAL_H_PROTO(TYPE, SUPER, FUNC_RETURN, FUNC_NAME, FUNC_ARG, FUNC_CALL) \
		inline FUNC_RETURN FUNC_NAME FUNC_ARG override { SUPER::FUNC_NAME FUNC_CALL; TYPE::I##FUNC_NAME FUNC_CALL; TYPE::Construct(); } \
 	private: \
		FUNC_RETURN I##FUNC_NAME FUNC_ARG;

#define SERIALIZABLE_CHECK_HEADER() \
	char magic[SERIALIZABLE_MAGIC_LENGTH]; \
	Read<char>(in, SERIALIZABLE_MAGIC_LENGTH, magic); \
	GE_ASSERT(strcmpb(magic, MAGIC, SERIALIZABLE_MAGIC_LENGTH), "UNEXPECTED MAGIC!"); \
	u8 version = Read<u8>(in);

// Implementation
#define SERIALIZABLE_PROTO(MAGIC_VAL, VERSION_VAL, TYPE, SUPER) \
	public: \
		explicit TYPE(istream& in, SETTINGS_T s) : SUPER(in, s) { SERIALIZABLE_CHECK_HEADER(); ISerialize(in, s); } \
		TYPE() = default; \
		static const constexpr char MAGIC[SERIALIZABLE_MAGIC_LENGTH + 1] = #MAGIC_VAL; \
		typedef SUPER::SETTINGS_T SETTINGS_T;\
		inline void Serialize(istream& in, SETTINGS_T s) override { *this = MOVE(TYPE(in, s)); } \
		inline void Deserialize(ostream& out) const override { SUPER::Deserialize(out); Write(out, SERIALIZABLE_MAGIC_LENGTH, MAGIC); Write<u8>(out, VERSION_VAL); IDeserialize(out); } \
	private: \
		void ISerialize(istream& in, SETTINGS_T s); \
		void IDeserialize(ostream& out) const;

#define SERIALIZABLE_REFLECTABLE(TYPE) \
 	public: \
		static TYPE* TYPE##FACTORY(std::istream& in, SETTINGS_T t) { return new TYPE(in, t); } \
		static GLOBAL TypeSystem<SETTINGS_T>::Type Type{ #TYPE, (TypeSystem<SETTINGS_T>::FactoryFunction) TYPE##FACTORY };

template<class T>
struct Serializable
{
	Serializable() = default;
	Serializable(istream&, T) { }
	DEFAULT_CM_CONSTRUCTOR(Serializable);

	typedef T SETTINGS_T;

	virtual void Serialize(istream& in, SETTINGS_T settings) {};
	virtual void Deserialize(ostream& out) const {};

	virtual ~Serializable() = default;
};

struct IFileContainer : public Serializable<gE::Window*>
{
	SERIALIZABLE_PROTO(FILE, 1, IFileContainer, Serializable<gE::Window*>);

 public:
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
	T& Cast() { T* t = Cast<T>(); GE_ASSERT(t, "NO T!"); return *t; }

	GET_CONST(const std::string&, Name, _name);
	GET_CONST(const TypeSystem<gE::Window*>::TypeID&, Type, _type);
	GET(Serializable<gE::Window*>&,, *_t);

 private:
	std::string _name;
	TypeSystem<gE::Window*>::TypeID _type;
	Serializable* _t;
};

template<is_serializable<gE::Window*> T>
struct FileContainer : public IFileContainer
{
public:
	template<typename ... ARGS>
	explicit FileContainer(std::string& path, ARGS&&... args) :
		IFileContainer(Object, path),
		Object(std::forward<ARGS>(args)...)
	{};

	explicit FileContainer(std::string& path) : IFileContainer(Object, path) {};

	T Object;
};

template<is_serializable_in<gE::Window*> T>
void ReadSerializableFromFile(gE::Window* window, const Path& path, T& t)
{
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	GE_ASSERT(file.is_open(), "COULD NOT OPEN FILE!");

	t.Serialize(file, window);

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
	t.Deserialize(file);
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
Array<T> ReadArray(std::istream& in)
{
	UINT_T length = Read<UINT_T>(in);
	Array<T> arr = Array<T>(length);
	in.read((char*) arr.Data(), length * sizeof(typename Array<T>::I));
	return arr;
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
		for(u32 i = 0; i < count; i++) t[i].Deserialize(out);
}

template<typename UINT_T, class T>
void ReadArraySerializable(std::istream& in, u32 count, Array<T>* t, typename T::SETTINGS_T s)
{
	for(u32 i = 0; i < count; i++)
	{
		Array<T>& t = t[i];

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