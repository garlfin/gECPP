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
	Type() = default;
	Type(const std::string_view name, FactoryFunction<T> factory) :
		Name(name),
		Factory(factory)
	{
		TypeSystem<T>::_types.insert(this);
	}

	std::string_view Name;
	size_t Offset;
	FactoryFunction<T> Factory;
};

struct IReflectable
{
	IReflectable() = default;
	virtual void OnEditorGUI(u8 depth) {};
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

#define REFLECTABLE_TYPE_PROTO(TYPE, NAME) \
	public: \
		using THIS_T = TYPE; \
		static TYPE* TYPE##FACTORY(std::istream& in, SETTINGS_T t); \
		const TYPE_T* GetType() const override { return &Type; }; \
		FORCE_IMPL static inline const TYPE_T Type{ NAME, (FactoryFunction<SETTINGS_T>) TYPE##FACTORY }; \

#ifdef DEBUG
	#define REFLECTABLE_ONGUI_PROTO(SUPER) \
		public: \
			void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); IOnEditorGUI(depth); } \
		private: \
			void IOnEditorGUI(u8 depth)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, CODE) \
		void TYPE::IOnEditorGUI(u8 depth) { CODE }
#else
	#define REFLECTABLE_ONGUI_PROTO(SUPER)
	#define REFLECTABLE_ONGUI_IMPL(TYPE, CODE) \
		NO_IMPL void TYPE##REFL_ONGUI_NOIMPL() {};
#endif

#define REFLECTABLE_PROTO(TYPE, SUPER, NAME) \
	REFLECTABLE_TYPE_PROTO(TYPE, NAME); \
	REFLECTABLE_ONGUI_PROTO(SUPER)

#define REFLECTABLE_PROTO_NOIMPL(SUPER) \
	public: \
		void OnEditorGUI(u8 depth) override { SUPER::OnEditorGUI(depth); }

#define REFLECTABLE_FACTORY_IMPL(TYPE, CONSTRUCTION_T) \
	TYPE* TYPE::TYPE##FACTORY(std::istream& in, gE::Window* t) { return (TYPE*) new CONSTRUCTION_T(in, t); }

#define REFLECTABLE_FACTORY_NO_IMPL(TYPE) \
	TYPE* TYPE::TYPE##FACTORY(std::istream& in, TYPE::SETTINGS_T t) { return nullptr; }

#define REFLECTABLE_ONEDITOR_NO_IMPL(TYPE, SUPER) \
	void TYPE::OnEditorGUI(u8 depth) { SUPER::OnEditorGUI(depth); }

// Typesystem must be explicity instantiated.
template struct TypeSystem<gE::Window*>;

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
