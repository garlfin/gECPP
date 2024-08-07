//
// Created by scion on 8/16/2023.
//

#pragma once

#include <Prototype.h>
#include "Engine/Math/Math.h"
#include <Engine/Array.h>
#include <Engine/Binary/Binary.h>
#include <fstream>
#include <unordered_map>

// Defines SERIALIZABLE_PROTO_T
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

template<class T, class S> CONSTEXPR_GLOBAL bool is_serializable = std::is_base_of_v<Serializable<S>, T>;

template<class T = void> struct Serializable;
template<class T> requires is_serializable<T, gE::Window*> struct FileContainer;

template<class T> requires is_serializable<T, gE::Window*> void ReadSerializableFromFile(gE::Window*, const char*, T&);
template<class T> requires is_serializable<T, gE::Window*> T* ReadSerializableFromFile(gE::Window*, const char*);
template<class T> requires is_serializable<T, gE::Window*> void WriteSerializableToFile(const char*, const T&);

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
template<class T> void ReadSerializable(std::istream& in, Serializable<T>& t, typename T::SETTINGS_T s) { ReadSerializable(in, 1, &t, s); }

template<> void Read(std::istream& in, u32 count, std::string* t);
template<> void Write(std::ostream& out, u32 count, const std::string* t);

#define VIRTUAL_H(FUNC_RETURN, FUNC) \
	inline FUNC_RETURN FUNC override {};

#define VIRTUAL_H_PROTO(TYPE, SUPER, FUNC_RETURN, FUNC_NAME, FUNC_ARG, FUNC_CALL) \
		inline FUNC_RETURN FUNC_NAME FUNC_ARG override { SUPER::FUNC_NAME FUNC_CALL; TYPE::I##FUNC_NAME FUNC_CALL; TYPE::Construct(); } \
 	private: \
		FUNC_RETURN I##FUNC_NAME FUNC_ARG;

#define SERIALIZABLE_PROTO_T_BODY(TYPE, SUPER) \
	public: \
		inline void Serialize(istream& in, SETTINGS_T s) override { SUPER::Serialize(in, s); TYPE::ISerialize(in, s); TYPE::Construct(); } \
		inline void Deserialize(ostream& out) const override { SUPER::Deserialize(out); TYPE::IDeserialize(out); } \
	private: \
		void ISerialize(istream& in, SETTINGS_T s); \
		void IDeserialize(ostream& out) const;

#define SERIALIZABLE_PROTO_T_CONSTRUCTABLE(TYPE, SUPER) \
	public: explicit TYPE(istream& in, SETTINGS_T s) : SUPER(in, s) { ISerialize(in, s); TYPE::Construct(); } \
    SERIALIZABLE_PROTO_T_BODY(TYPE, SUPER)

#define SERIALIZABLE_PROTO_T(TYPE, SUPER) \
 	private: ALWAYS_INLINE void Construct() {}; \
	SERIALIZABLE_PROTO_T_CONSTRUCTABLE(TYPE, SUPER);

#define SERIALIZABLE_PROTO_BODY(TYPE, SUPER) \
	public: \
		inline void Serialize(istream& in) override { SUPER::Serialize(in); TYPE::ISerialize(in); TYPE::Construct(); } \
		inline void Deserialize(ostream& out) const override { SUPER::Deserialize(out); TYPE::IDeserialize(out); } \
	private: \
		void ISerialize(istream& in); \
		void IDeserialize(ostream& out) const;

#define SERIALIZABLE_PROTO_CONSTRUCTABLE(TYPE, SUPER) \
	public: \
		explicit TYPE(istream& in) : SUPER(in) { ISerialize(in); TYPE::Construct(); } \
    SERIALIZABLE_PROTO_BODY(TYPE, SUPER)

#define SERIALIZABLE_PROTO(TYPE, SUPER) \
	private: ALWAYS_INLINE void Construct() {}; \
	SERIALIZABLE_PROTO_CONSTRUCTABLE(TYPE, SUPER);

#define SERIALIZABLE_REFLECTABLE(TYPE) \
 	public: \
		static TYPE* TYPE##FACTORY(std::istream& in, SETTINGS_T t) { return new TYPE(in, t); } \
		static GLOBAL TypeSystem<SETTINGS_T>::Type Type{ #TYPE, (TypeSystem<SETTINGS_T>::FactoryFunction) TYPE##FACTORY };

template<class T>
struct Serializable
{
	Serializable() = default;
	Serializable(istream&, T) { }

	typedef T SETTINGS_T;

	virtual void Deserialize(ostream& ptr) const {};
	virtual void Serialize(istream& ptr, T s) {};

	virtual ~Serializable() = default;
};

template<>
struct Serializable<void>
{
	Serializable() = default;
	explicit Serializable(istream& ptr) { }

	virtual void Deserialize(ostream& ptr) const {};
	virtual void Serialize(istream& ptr) {};

	virtual ~Serializable() = default;
};

struct IFileContainer : public Serializable<gE::Window*>
{
	SERIALIZABLE_PROTO_T(IFileContainer, Serializable<gE::Window*>);

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
	T& Cast() { T* t = Cast<T>(); assert(t); return *t; }

	GET_CONST(const std::string&, Name, _name);
	GET_CONST(const TypeSystem<gE::Window*>::TypeID&, Type, _type);
	GET(Serializable<gE::Window*>&,, *_t);

 private:
	std::string _name;
	TypeSystem<gE::Window*>::TypeID _type;
	Serializable<gE::Window*>* _t;
};

template<class T> requires is_serializable<T, gE::Window*>
struct FileContainer : public IFileContainer
{
	template<typename ... ARGS>
	explicit FileContainer(std::string& path, ARGS&&... args) :
		IFileContainer(Object, path),
		Object(std::forward<ARGS>(args)...)
	{};

	explicit FileContainer(std::string& path) : IFileContainer(Object, path) {};

	T Object;
};

template<class T> requires is_serializable<T, gE::Window*>
void ReadSerializableFromFile(gE::Window* w, const char* path, T& t)
{
	std::ifstream file;
	file.open(path, std::ios::in | std::ios::binary);
	assert(file.is_open());

	t.Serialize(file, w);

	file.close();
}

template<class T> requires is_serializable<T, gE::Window*>
T* ReadSerializableFromFile(gE::Window* w, const char* path)
{
	T* t = new T();
	ReadSerializableFromFile(w, path, *t);
	return t;
}

template<class T> requires is_serializable<T, gE::Window*>
void WriteSerializableToFile(const char* path, const T& t)
{
	std::ofstream file;
	file.open(path, std::ios::out | std::ios::binary);

	t.Deserialize(file);

	file.close();
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
	in.read((char*) arr.Data(), length * sizeof(T));

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
void Read(istream& src, u32 count, T* ts)
{
	if(!count) return;
	if constexpr(std::is_trivially_copyable_v<T>)
		src.read((char*) ts, sizeof(T) * count);
	else
		for(u32 i = 0; i < count; i++) ts[i].Serialize(src);
}

template<typename T>
void Write(ostream& out, u32 count, const T* ts)
{
	if(!count) return;
	if constexpr(std::is_trivially_copyable_v<T>)
		out.write((char*) ts, sizeof(T) * count);
	else
		for(u32 i = 0; i < count; i++) ts[i].Deserialize(out);
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

template<class T> void ReadSerializable(std::istream& in, u32 count, T* t, typename T::SETTINGS_T s)
{
	if(!count) return;
	for(u32 i = 0; i < count; i++) t[i].Serialize(in, s);
}