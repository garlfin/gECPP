//
// Created by scion on 2/13/2025.
//

#pragma once

#include <Prototype.h>
#include <set>
#include <Core/Macro.h>
#include <Core/Math/Math.h>

template<class T>
struct Type;

template<class T>
using FactoryFunction = Serializable<gE::Window*>*(*)(std::istream&, T);

template<class T>
using UFactoryFunction = void(*)(std::istream&, T, Serializable<gE::Window*>&);

template<class SERIALIZABLE_T>
concept ReflConstructible = requires
{
	SERIALIZABLE_T::Type;
};

template<class T>
struct TypeCompare
{
	using is_transparent = void;
	using TYPE_T = Type<T>*;
	using KEY_T = std::string_view;

	NODISCARD ALWAYS_INLINE bool operator()(const TYPE_T& a, const TYPE_T& b) const;
	NODISCARD ALWAYS_INLINE bool operator()(const TYPE_T& a, const KEY_T& b) const;
	NODISCARD ALWAYS_INLINE bool operator()(const KEY_T& a, const TYPE_T& b) const;
};

template<class T>
struct TypeSystem
{
	TypeSystem() = delete;

	using SET_T = std::set<Type<T>*, TypeCompare<T>>;

	static const Type<T>* GetTypeInfo(const std::string_view& type)
	{
		typename SET_T::const_iterator it = _types.find(type);

		if(it != _types.end()) return *it;

		GE_FAIL("NO SUCH REFLECTED TYPE!");
		return nullptr;
	}

	NODISCARD static ALWAYS_INLINE const SET_T& GetTypes() { return _types; };

	friend struct Type<T>;

private:
	static inline SET_T _types = DEFAULT;
};

template<class T>
struct Type
{
	Type() = delete;
	explicit Type(std::string_view name, FactoryFunction<T> factory, UFactoryFunction<T> uFactory, std::string_view extension, const Type* baseType = nullptr) :
		Name(name),
		Factory(factory),
		UFactory(uFactory),
		BaseType(baseType)
	{
		Extension.replace_extension(extension);
		TypeSystem<T>::_types.insert(this);
	}

	NODISCARD const Type* GetBaseType() const { return BaseType ? BaseType : this; }

	std::string_view Name;
	Path Extension;
	FactoryFunction<T> Factory;
	UFactoryFunction<T> UFactory;
	const Type* BaseType;
};

struct IReflectable
{
	IReflectable() = default;
#ifdef GE_ENABLE_EDITOR
	virtual void OnEditorGUI(u8 depth) {};
	virtual bool OnEditorIcon(Size1D size) { return false; }
#endif
	virtual ~IReflectable() = default;
};

template<class T>
struct Reflectable : public IReflectable
{
public:
	Reflectable() = default;

	using SETTINGS_T = T;
	using TYPE_T = Type<T>;
	using TSYSTEM_T = TypeSystem<T>;

	virtual const Type<T>* GetType() const { return nullptr; }
	void OnEditorGUI(u8 depth) override { IReflectable::OnEditorGUI(depth); }

	~Reflectable() override = default;
};

enum class EnumType : u8
{
	Normal,
	Bitfield
};

template<class T, size_t SIZE>
struct EnumData
{
	static_assert(std::is_enum_v<T>);
	using PAIR_T = std::pair<T, std::string_view>;
	using ARR_T = std::array<PAIR_T, SIZE>;
	using INT_T = std::underlying_type_t<T>;

	EnumType Type = EnumType::Normal;
	ARR_T Enums = DEFAULT;

	NODISCARD std::string ToString(T t) const;
	NODISCARD typename ARR_T::const_iterator At(T t) const;
};

#define REFLECTABLE_TYPE_PROTO(TYPE, NAME, ...) \
	public: \
		using THIS_T = TYPE; \
		static TYPE* Factory(std::istream& in, SETTINGS_T t); \
		static void UFactory(std::istream& in, SETTINGS_T t, TYPE& result); \
		const TYPE_T* GetType() const override { return &Type; }; \
		FORCE_IMPL static inline const TYPE_T Type{ NAME, (FactoryFunction<SETTINGS_T>) Factory, (UFactoryFunction<SETTINGS_T>) UFactory, MAGIC, __VA_ARGS__ }; \

#ifdef GE_ENABLE_IMGUI
	#define REFLECTABLE_ONGUI_PROTO(SUPER) \
		public: \
			void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); IOnEditorGUI(depth); } \
		private: \
			void IOnEditorGUI(u8 depth)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, ...) \
		void TYPE::IOnEditorGUI(u8 depth) { __VA_ARGS__ }
	#define REFLECTABLE_ICON_PROTO() \
		bool OnEditorIcon(Size1D size) override
	#define REFLECTABLE_ICON_IMPL(TYPE, ...) \
		bool TYPE::OnEditorIcon(Size1D size) { __VA_ARGS__; return true; }
#else
	#define REFLECTABLE_ONGUI_PROTO(SUPER)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, ...)
	#define REFLECTABLE_ICON_PROTO
	#define REFLECTABLE_ICON_IMPL(TYPE, ...)
#endif

#define REFLECTABLE_MAGIC_IMPL(MAGIC_VAL) \
	public: \
		static const constexpr char MAGIC[5] = MAGIC_VAL

#define REFLECTABLE_PROTO(TYPE, SUPER, NAME, ...) \
	REFLECTABLE_TYPE_PROTO(TYPE, NAME, __VA_ARGS__); \
	REFLECTABLE_ONGUI_PROTO(SUPER)

#define REFLECTABLE_PROTO_NOIMPL(SUPER) \
	public: \
		void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); }

#define REFLECTABLE_FACTORY_IMPL(TYPE, CONSTRUCTION_T, ...) \
	__VA_ARGS__ TYPE* TYPE::Factory(std::istream& in, TYPE::SETTINGS_T t) { return (TYPE*) new CONSTRUCTION_T(in, t); } \
	__VA_ARGS__ void TYPE::UFactory(std::istream& in, TYPE::SETTINGS_T t, TYPE& result) { PlacementNew((CONSTRUCTION_T&) result, in, t); }

#define REFLECTABLE_FACTORY_NO_IMPL(TYPE, ...) \
	__VA_ARGS__ TYPE* TYPE::Factory(std::istream&, TYPE::SETTINGS_T) { return nullptr; } \
	__VA_ARGS__ void TYPE::UFactory(std::istream&, TYPE::SETTINGS_T, TYPE&) { }

#define REFLECTABLE_ONEDITOR_NO_IMPL(TYPE, SUPER) \
	void TYPE::OnEditorGUI(u8 depth) { SUPER::OnEditorGUI(depth); }

#define REFLECTABLE_ENUM(ETYPE, ENUM, SIZE, ...) \
	CONSTEXPR_GLOBAL EnumData<ENUM, SIZE> E##ENUM \
	{ \
		EnumType::ETYPE, \
		{ \
			__VA_ARGS__ \
		} \
	};

#define ENUM_DEF(E_TYPE, E_NAME) ENUM_PAIR(E_TYPE::E_NAME, #E_NAME)
#define ENUM_PAIR(E_VAL, E_NAME) std::make_pair(E_VAL, E_NAME##sv)

// Typesystem must be explicity instantiated.
template struct TypeSystem<gE::Window*>;
template struct TypeSystem<std::nullptr_t>;

template <class T>
bool TypeCompare<T>::operator()(const TYPE_T& a, const TYPE_T& b) const
{
	return a->Name < b->Name;
}

template <class T>
bool TypeCompare<T>::operator()(const TYPE_T& a, const KEY_T& b) const
{
	return a->Name < b;
}

template <class T>
bool TypeCompare<T>::operator()(const KEY_T& a, const TYPE_T& b) const
{
	return a < b->Name;
}

#include "Reflectable.inl"