//
// Created by scion on 2/13/2025.
//

#pragma once

#include <unordered_map>
#include <Math/Math.h>
#include <Utility/Macro.h>
#include <Prototype.h>

enum class FieldType : u16
{
	None,
	Bool,
	I32,
	U32,
	Float,
	String,
	Reflectable,
	Vec2,
	Vec3,

	ReadOnly = 1 << 15,
	Array = 1 << 14,
	Pointer = 1 << 13,
	Mods = ReadOnly | Array | Pointer,
};

ENUM_OPERATOR(FieldType, &);
ENUM_OPERATOR(FieldType, |);
ENUM_OPERATOR_UNARY(FieldType, ~);

template<class T>
CONSTEXPR_GLOBAL FieldType GetFieldType();

struct Type;

struct TypeSystem
{
	TypeSystem() = delete;

	using FactoryFunction = Serializable<gE::Window*>*(*)(std::istream&, gE::Window*);

	using MAP_T = std::unordered_map<std::string_view, Type*>;

	static const Type* GetTypeInfo(const std::string& type)
	{
		MAP_T::const_iterator it = _types.find(type);

		if(it != _types.end()) return it->second;

		GE_FAIL("NO SUCH REFLECTED TYPE!");
		return nullptr;
	}

	friend struct Type;

private:
	static inline MAP_T _types {};
};

struct Field
{
	std::string_view Name;
	FieldType Type;
	size_t Offset;

	void* AtOffset(Reflectable* t) const { return (u8*) t + Offset; }
	const void* AtOffset(const Reflectable* t) const { return (const u8*) t + Offset;};
};

struct Type
{
	Type() = default;
	Type(const std::string_view& name, TypeSystem::FactoryFunction factory, const Type* parentType, const std::initializer_list<Field>& fields) :
		Name(name),
		Factory(factory)
#ifdef DEBUG
		, Fields(fields)
#endif
	{
	#ifdef DEBUG
		if(parentType)
			Fields.insert(Fields.begin(), parentType->Fields.begin(), parentType->Fields.end());
	#endif

		TypeSystem::_types[Name] = this;
	}

	template<class PARENT_T = void>
	static constexpr const Type* GetParentType()
	{
		if constexpr(requires() { { PARENT_T::SGetType() } -> std::same_as<const Type&>; })
			return &PARENT_T::SGetType();

		return nullptr;
	}

	std::string_view Name;
	size_t Offset;
	TypeSystem::FactoryFunction Factory;

#ifdef DEBUG
	std::vector<Field> Fields;
#endif
};

struct Reflectable
{
public:
	Reflectable() = default;

	using SETTINGS_T = gE::Window*;

	virtual const Type* GetType() const { return nullptr; }
	virtual void OnEditorGUI() {};

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
	const size_t offset = (size_t) &((BASE*) nullptr)->_refl_baseOffset_ - sizeof(u64);
	GE_ASSERTM(offset == 0, "Reflectable must be first inherited.");
	return offset;
}

#define REFLECTABLE_BEGIN(TYPE) \
	FORCE_IMPL const Type& _REFL_IMPL_TYPE_##TYPE();

#define REFLECTABLE_PROTO(TYPE) \
	public: \
		using THIS_T = TYPE; \
		static TYPE* TYPE##FACTORY(std::istream& in, gE::Window* t); \
		const Type* GetType() const override { return &_REFL_IMPL_TYPE_##TYPE(); }; \
		static const Type& SGetType() { return _REFL_IMPL_TYPE_##TYPE(); } \
		friend const Type& _REFL_IMPL_TYPE_##TYPE();

#ifdef DEBUG
	#define REFLECTABLE_END(TYPE, PARENT_T, NAME, ...) \
		const Type& _REFL_IMPL_TYPE_##TYPE() { static const Type type(NAME, (TypeSystem::FactoryFunction) TYPE::TYPE##FACTORY, Type::GetParentType<PARENT_T>(), { __VA_ARGS__ }); return type; } \
		FORCE_IMPL GLOBAL Type& _REFL_TYPE_##TYPE = _REFL_IMPL_TYPE_##TYPE();
#else
	#define REFLECTABLE_END(TYPE, PARENT_T, NAME, ...) \
		const Type& _REFL_IMPL_TYPE_##TYPE() { static const Type type(NAME, (TypeSystem::FactoryFunction) TYPE::TYPE##FACTORY, , { }); return type; } \
		FORCE_IMPL GLOBAL Type& _REFL_TYPE_##TYPE = _REFL_IMPL_TYPE_##TYPE();
#endif

#define REFLECTABLE_FACTORY_IMPL(TYPE, CONSTRUCTION_T) \
	TYPE* TYPE::TYPE##FACTORY(std::istream& in, gE::Window* t) { return (TYPE*) new CONSTRUCTION_T(in, t); }

#define REFLECTABLE_FACTORY_NO_IMPL(TYPE) \
	TYPE* TYPE::TYPE##FACTORY(std::istream& in, gE::Window* t) { return nullptr; }

#define REFLECT_FIELD(OWNER_T, FIELD) \
	Field(#FIELD, GetFieldType<decltype(OWNER_T::FIELD)>(), offsetof_memptr(OWNER_T, &OWNER_T::FIELD) - ReflectedBaseOffset<OWNER_T>())

template <class T>
constexpr FieldType GetFieldType()
{
	using RAW_T = std::remove_cvref_t<std::remove_pointer_t<T>>;

	FieldType type = FieldType::None;
	if constexpr(std::is_same_v<RAW_T, bool>)
		type = FieldType::Bool;
	else if constexpr(std::is_same_v<RAW_T, std::string>)
		type = FieldType::String;
	else if constexpr(std::is_same_v<RAW_T, float>)
		type = FieldType::Float;
	else if constexpr(std::is_same_v<RAW_T, i32>)
		type = FieldType::I32;
	else if constexpr(std::is_same_v<RAW_T, u32>)
		type = FieldType::U32;
	else if constexpr(std::is_base_of_v<Reflectable, RAW_T>)
		type = FieldType::Reflectable;
	else
		static_assert(std::is_pointer_v<T>);

	if constexpr(std::is_pointer_v<T>)
		type |= FieldType::Pointer;

	return type;
}
