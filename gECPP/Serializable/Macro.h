//
// Created by scion on 9/5/2024.
//

#pragma once

// TODO: Find better alternative to creating a copy of the settings.
// Prevents _settings from being initialied to 0.

#define API_SERIALIZABLE(TYPE, SUPER_T)\
	public: \
		typedef SUPER_T::SETTINGS_T SETTINGS_T; \
		typedef SUPER_T SUPER; \
		inline void Serialize(istream& in, gE::Window* window) override { SAFE_CONSTRUCT(*this, TYPE, in, window); } \
		TYPE() = default; \
		TYPE(istream& in, gE::Window* window) \
		{ \
			SUPER_T tmp(in, window); \
			new(this) TYPE(window, move(tmp)); \
			SUPER_T::Free(); \
		} \
		GET(SUPER_T&, Settings, *this) \
		TYPE(gE::Window* window, const SUPER_T& settings) : TYPE(window, (SUPER_T&&) SUPER_T(settings)) {} \
		TYPE(gE::Window* window, SUPER_T&& INTERNAL_SETTINGS)

#define API_DEFAULT_CM_CONSTRUCTOR(TYPE) \
	public: \
		TYPE(TYPE&&) = default; \
		TYPE& operator=(TYPE&&) = default; \
		TYPE(const TYPE&) = delete; \
		TYPE& operator=(const TYPE&) = delete

#define API_REFLECTABLE(TYPE, NAME, ...) \
	public: \
		static __VA_ARGS__* TYPE##FACTORY(std::istream& in, SETTINGS_T t); \
		static GLOBAL typename ITypeSystem<SETTINGS_T>::Type Type{ NAME, (typename ITypeSystem<SETTINGS_T>::FactoryFunction) TYPE##FACTORY }; \
		const typename ITypeSystem<SETTINGS_T>::Type* GetType() const { return &Type; }

#define API_REFLECTABLE_IMPL(TYPE, API_TYPE) \
	inline API_TYPE* TYPE::TYPE##FACTORY(std::istream& in, SETTINGS_T t) { return new API_TYPE(in, t); }

#define API_UNDERLYING() \
	public: \
		virtual inline void* GetUnderlying() { return nullptr; } \
		virtual inline const void* GetUnderlying() const { return nullptr; }

#define API_UNDERLYING_IMPL(UNDERLYING_T) \
	public: \
		inline void* GetUnderlying() override { return (UNDERLYING_T*) this; } \
		inline const void* GetUnderlying() const override { return (const UNDERLYING_T*) this; }
