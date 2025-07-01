//
// Created by scion on 2/13/2025.
//

#pragma once

#include "Field.h"

#include <array>
#include <Prototype.h>
#include <set>
#include <Core/Macro.h>
#include <Core/Math/Math.h>

class IField;

template<class T> requires std::is_convertible_v<std::remove_cvref_t<T>, gE::Window*>
struct Type;

template<class T>
using FactoryFunction = Serializable<gE::Window*>*(*)(std::istream&, T);

template<class T>
using UFactoryFunction = void(*)(std::istream&, T, Serializable<gE::Window*>&);

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
class TypeSystem
{
public:
	TypeSystem() = delete;

	using SET_T = std::set<Type<T>*, TypeCompare<T>>;

	static const Type<T>* GetTypeInfo(const std::string_view& type)
	{
		typename SET_T::const_iterator it = _types.find(type);

		if(it != _types.end()) return *it;

		GE_FAIL("NO SUCH REFLECTED TYPE!");
		return nullptr;
	}

	static const Type<T>* GetTypeInfoFromExtension(const Path& extension)
	{
		auto it = std::ranges::find_if(_types, [&](const Type<T>* type)
		   {
			   return extension == type->Extension;
		   });

		return it == _types.end() ? nullptr : *it;
	}

	NODISCARD static ALWAYS_INLINE const SET_T& GetTypes() { return _types; };

	friend struct Type<T>;

private:
	static inline SET_T _types = DEFAULT;
};

template<class T> requires std::is_convertible_v<std::remove_cvref_t<T>, gE::Window*>
struct Type
{
private:
	using VEC_T = std::vector<IField*>;

public:
	Type() = delete;

	explicit Type(const std::string& name, const std::string& extension, FactoryFunction<T> factory, VEC_T&& fields, u8 version = 0, const Type* baseType = nullptr) :
		Name(name),
		Magic(extension),
		Factory(factory),
		Version(version),
		Fields(std::move(fields)),
		BaseType(baseType)
	{
		Extension.replace_extension(extension);
		TypeSystem<T>::_types.insert(this);
	}

	template<class I, typename... ARGS>
	static Type MakeType(const std::string& extension, FactoryFunction<T> factory, VEC_T&& fields, ARGS&&... args)
	{
		return Type(demangle(typeid(I).name()), extension, factory, std::move(fields), std::forward<ARGS>(args)...);
	}

	NODISCARD const Type* GetBaseType() const { return BaseType ? BaseType : this; }

	std::string Name;
	std::string Magic;
	Path Extension;
	FactoryFunction<T> Factory;
	u8 Version;
	VEC_T Fields;
	const Type* BaseType;

	~Type() { for(IField* field : Fields) delete field; }
};

struct IReflectable
{
	IReflectable() = default;
#ifdef GE_ENABLE_EDITOR
	virtual void OnEditorGUI(u8 depth) {};
	virtual bool OnEditorIcon(Size1D size) { return false; }
	virtual std::string GetEditorName() const { return std::format("{}", (const void*) this); }
#endif

	template<class I>
	NODISCARD bool IsCastable() const
	{
		return dynamic_cast<const I*>(this);
	}

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

#define REFLECTABLE_TYPE_PROTO(TYPE) \
	public: \
		using THIS_T = TYPE; \
		static TYPE* Factory(std::istream& in, SETTINGS_T t); \
		const TYPE_T* GetType() const override { return &SType; }; \
		static const TYPE_T SType;

#define REFLECTABLE_CLASS(TYPE, ...) \
	const TYPE::TYPE_T TYPE::SType = TYPE_T::MakeType<TYPE>(#TYPE, (FactoryFunction<SETTINGS_T>) Factory __VA_OPT__(,) __VA_ARGS__);

#define REFLECT_MEMBER(SETTINGS_TYPE, MEMBER, FLAGS, TOOLTIP, ...) \
	new SETTINGS_TYPE(&THIS_T::MEMBER, gE::Editor::SETTINGS_TYPE{ #MEMBER, TOOLTIP __VA_OPT__(,) __VA_ARGS__}, FLAGS)

#ifdef GE_ENABLE_IMGUI
	#define REFLECTABLE_ONGUI_PROTO(SUPER) \
		public: \
			void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); IOnEditorGUI(depth); } \
		private: \
			void IOnEditorGUI(u8 depth)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, ...) \
			void TYPE::IOnEditorGUI(u8 depth) { __VA_ARGS__ }
	#define REFLECTABLE_ICON_PROTO() \
		public: \
			bool OnEditorIcon(Size1D size) override
	#define REFLECTABLE_NAME_PROTO() \
		public: \
			std::string GetEditorName() const override
	#define REFLECTABLE_ICON_IMPL(TYPE, ...) \
		bool TYPE::OnEditorIcon(Size1D size) { __VA_ARGS__; return true; }
	#define REFLECTABLE_NAME_IMPL(TYPE, ...) \
		std::string TYPE::GetEditorName() const { __VA_ARGS__; }
#else
	#define REFLECTABLE_ONGUI_PROTO(SUPER)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, ...)
	#define REFLECTABLE_ICON_PROTO()
	#define REFLECTABLE_NAME_PROTO()
	#define REFLECTABLE_ICON_IMPL(TYPE, ...)
	#define REFLECTABLE_NAME_IMPL(TYPE, ...)
#endif

#define REFLECTABLE_PROTO(TYPE, SUPER) \
	REFLECTABLE_TYPE_PROTO(TYPE); \
	REFLECTABLE_ONGUI_PROTO(SUPER)

#define REFLECTABLE_NOIMPL(SUPER) \
	public: \
		void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); }

#define REFLECTABLE_FACTORY_IMPL(TYPE, ...) \
	__VA_ARGS__ TYPE* TYPE::Factory(std::istream& in, TYPE::SETTINGS_T t) { return (TYPE*) new TYPE(in, t); }

#define REFLECTABLE_FACTORY_NO_IMPL(TYPE, ...) \
	__VA_ARGS__ TYPE* TYPE::Factory(std::istream&, TYPE::SETTINGS_T) { return nullptr; }

#define REFLECTABLE_ONEDITOR_NO_IMPL(TYPE, SUPER) \
	void TYPE::OnEditorGUI(u8 depth) { SUPER::OnEditorGUI(depth); }

/**
 *
 * @param ETYPE EnumType: View mode (Normal, Bitfield)
 * @param ENUM Enum
 * @param SIZE Number of enums
 */
#ifdef GE_ENABLE_IMGUI
	#define REFLECTABLE_ENUM(ETYPE, ENUM, SIZE, ...) \
		CONSTEXPR_GLOBAL EnumData<ENUM, SIZE> E##ENUM \
		{ \
			EnumType::ETYPE, \
			{ \
				__VA_ARGS__ \
			} \
		}
#else
	#define REFLECTABLE_ENUM(ETYPE, ENUM, SIZE, ...)
#endif

#define REFLECT_ENUM(E_TYPE, E_NAME) ENUM_PAIR(E_TYPE::E_NAME, #E_NAME)
#define ENUM_PAIR(E_VAL, E_NAME) std::make_pair(E_VAL, E_NAME##sv)

// Typesystem must be explicitly instantiated.
template class TypeSystem<gE::Window*>;
template class TypeSystem<std::nullptr_t>;

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