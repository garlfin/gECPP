//
// Created by scion on 8/16/2023.
//

#pragma once

#include <fstream>
#include <Prototype.h>
#include <unordered_map>
#include <Utility/Array.h>
#include <Utility/Binary.h>
#include <Utility/Macro.h>
#include <Math/Math.h>
#include "Macro.h"

enum class FieldType : u8
{
	None,
	Bool,
	I32,
	U32,
	Float,
	Pointer,
	String,
	Reflectable,
};

enum class FieldFlags : u8
{
	ReadOnly = 1 << 1,
	Array = 1 << 2
};

ENUM_OPERATOR(FieldType, &);
ENUM_OPERATOR(FieldType, |);

template<class T>
CONSTEXPR_GLOBAL FieldType GetFieldType();

template<class T>
struct ITypeSystem
{
	ITypeSystem() = delete;

	using FactoryFunction = Serializable<T>*(*)(std::istream&, T);

	struct Field
	{
		std::string_view Name;
		FieldType Type;
		size_t Offset;

		void* AtOffset(Reflectable<T>* t) const { return (u8*) t + Offset; }
		const void* AtOffset(const Reflectable<T>* t) const { return (const u8*) t + Offset;};
	};

	struct Type
	{
		Type() = default;
		Type(const std::string_view& name, FactoryFunction factory, const std::initializer_list<Field>& fields) :
			Name(name),
			Factory(factory),
			Fields(fields)
		{
			_types[Name] = this;
		}

		std::string_view Name;
		FactoryFunction Factory;

#ifdef DEBUG
		Array<Field> Fields;
#endif
	};

	using MAP_T = std::unordered_map<std::string_view, Type*>;

	static const Type* GetTypeInfo(const std::string& type)
	{
		typename MAP_T::const_iterator it = _types.find(type);

		if(it != _types.end()) return it->second;

		GE_FAIL("NO SUCH REFLECTED TYPE!");
		return nullptr;
	}

private:
	static inline MAP_T _types {};
};

template class ITypeSystem<gE::Window*>;
using TypeSystem = ITypeSystem<gE::Window*>;

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

template<class T> const typename ITypeSystem<T>::Type* ReadType(std::istream& in);
template<class T> void WriteType(std::ostream& out, const typename ITypeSystem<T>::Type& type);

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

#define SERIALIZABLE_CHECK_HEADER() \
	char magic[4]; \
	Read<char>(in, 4, magic); \
	GE_ASSERTM(strcmpb(magic, MAGIC, 4), "UNEXPECTED MAGIC!"); \
	Version = Read<u8>(in);

// Implementation
#define SERIALIZABLE_PROTO(MAGIC_VAL, VERSION_VAL, TYPE, SUPER_T) \
	public: \
		typedef SUPER_T SUPER; \
		typedef SUPER::SETTINGS_T SETTINGS_T;\
		TYPE(istream& in, SETTINGS_T s) : SUPER(in, s) { SERIALIZABLE_CHECK_HEADER(); IDeserialize(in, s); } \
		TYPE() = default; \
		static const constexpr char MAGIC[5] = MAGIC_VAL; \
		inline void Deserialize(istream& in, SETTINGS_T s) override { SAFE_CONSTRUCT(*this, TYPE, in, s); } \
		inline void Serialize(ostream& out) const override { SUPER::Serialize(out); Write(out, 4, MAGIC); Write<u8>(out, Version); ISerialize(out); } \
		u8 Version = VERSION_VAL; \
		DEFAULT_OPERATOR_CM(TYPE); \
	private: \
		void IDeserialize(istream& in, SETTINGS_T s); \
		void ISerialize(ostream& out) const;

#define REFLECTABLE_PROTO(TYPE) \
	public: \
		using THIS_T = TYPE; \
		static TYPE* TYPE##FACTORY(std::istream& in, SETTINGS_T t); \
		const ITypeSystem<SETTINGS_T>::Type* GetType() const override { return &SGetType(); }; \
		static const ITypeSystem<SETTINGS_T>::Type& SGetType()

#ifdef DEBUG
#define REFLECTABLE_IMPL(TYPE, NAME, ...) \
	FORCE_IMPL GLOBAL ITypeSystem<TYPE::SETTINGS_T>::Type _REFL_IMPL_TYPE_##TYPE{ NAME, (ITypeSystem<TYPE::SETTINGS_T>::FactoryFunction) TYPE::TYPE##FACTORY, { __VA_ARGS__ }}; \
	inline const ITypeSystem<TYPE::SETTINGS_T>::Type& TYPE::SGetType() { return _REFL_IMPL_TYPE_##TYPE; }
#else
#define REFLECTABLE_IMPL(TYPE, NAME, ...)
	FORCE_IMPL GLOBAL ITypeSystem<TYPE::SETTINGS_T>::Type _REFL_IMPL_TYPE_##TYPE{ NAME, (ITypeSystem<TYPE::SETTINGS_T>::FactoryFunction) TYPE::TYPE##FACTORY, {}; \
	inline const ITypeSystem<TYPE::SETTINGS_T>::Type& TYPE::SGetType() { return _REFL_IMPL_TYPE_##TYPE; }
#endif

#define REFLECTABLE_FACTORY_IMPL(TYPE, CONSTRUCTION_T) \
	TYPE* TYPE::TYPE##FACTORY(std::istream& in, SETTINGS_T t) { return (TYPE*) new CONSTRUCTION_T(in, t); }

template<class BASE>
constexpr size_t ReflectedBaseOffset();

#define REFLECT_FIELD(OWNER_T, FIELD) \
	ITypeSystem<OWNER_T::SETTINGS_T>::Field{ #FIELD, GetFieldType<decltype(OWNER_T::FIELD)>(), offsetof_memptr(OWNER_T, &OWNER_T::FIELD) - ReflectedBaseOffset<OWNER_T>()}

template<class T>
struct Reflectable
{
public:
	Reflectable() = default;

	using SETTINGS_T = T;

	virtual const typename ITypeSystem<T>::Type* GetType() const { return nullptr; }

	virtual ~Reflectable() = default;

	template<class I>
	friend constexpr size_t ReflectedBaseOffset();

private:
	u8 _refl_baseOffset_;
};

// TODO: better way of doing this?
template<class BASE>
constexpr size_t ReflectedBaseOffset()
{
	// + size of vtable
	return (size_t) &((BASE*) nullptr)->_refl_baseOffset_ - sizeof(u64);
}

template<class T>
struct Serializable : public Reflectable<T>
{
	DEFAULT_OPERATOR_CM(Serializable);

public:
	Serializable() = default;
	Serializable(istream&, T) { }

	using SETTINGS_T = T;

	virtual void Deserialize(istream& in, SETTINGS_T settings) {};
	virtual void Serialize(ostream& out) const {};

	virtual void* GetUnderlying() { return nullptr; }
	virtual const void* GetUnderlying() const { return nullptr; }

	~Serializable() override = default;
};

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

template <class T>
constexpr FieldType GetFieldType()
{
	if constexpr(std::is_same_v<T, bool>)
		return FieldType::Bool;
	else if constexpr(std::is_same_v<T, std::string>)
		return FieldType::String;
	else if constexpr(std::is_same_v<T, float>)
		return FieldType::Float;
	else if constexpr(std::is_same_v<T, i32>)
		return FieldType::I32;
	else if constexpr(std::is_same_v<T, u32>)
		return FieldType::U32;

	else if constexpr(std::is_pointer_v<T>)
		return FieldType::Pointer;
	else if constexpr(is_serializable_out<T>)
		return FieldType::Reflectable;

	else static_assert(false);
	return FieldType::None;
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

template<class T>
const typename ITypeSystem<T>::Type* ReadType(std::istream& in)
{
	return ITypeSystem<T>::GetTypeInfo(Read<std::string>(in));
}

template<class T>
void WriteType(std::ostream& out, const typename ITypeSystem<T>::Type& type)
{
	Write(out, type.Name);
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