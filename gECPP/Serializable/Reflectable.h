//
// Created by scion on 2/13/2025.
//

#pragma once

#include <unordered_map>
#include <Math/Math.h>
#include <Utility/Macro.h>
#include <Prototype.h>

template<class T>
struct Type;

template<class T>
using FactoryFunction = Serializable<gE::Window*>*(*)(std::istream&, T);

template<class T>
struct TypeSystem
{
	TypeSystem() = delete;

	using MAP_T = std::unordered_map<std::string_view, Type<T>*>;

	static const Type<T>* GetTypeInfo(const std::string& type)
	{
		typename MAP_T::const_iterator it = _types.find(type);

		if(it != _types.end()) return it->second;

		GE_FAIL("NO SUCH REFLECTED TYPE!");
		return nullptr;
	}

	friend struct Type<T>;

private:
	static inline MAP_T _types = DEFAULT;
};

template<class T>
struct Type
{
	Type() = default;
	Type(const std::string_view name, FactoryFunction<T> factory) :
		Name(name),
		Factory(factory)
	{
		TypeSystem<T>::_types[Name] = this;
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

	virtual ~Reflectable() = default;
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
#else
	#define REFLECTABLE_ONGUI_PROTO(SUPER)
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