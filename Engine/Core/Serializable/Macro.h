//
// Created by scion on 9/5/2024.
//

#pragma once

#include <Core/Macro.h>

// TODO: Find better alternative to creating a copy of the settings.
// Prevents _settings from being initialized to 0.

#define API_DEFAULT_CM_CONSTRUCTOR(TYPE) \
	public: \
		TYPE(TYPE&&) = default; \
		TYPE& operator=(TYPE&&) = default; \
		TYPE(const TYPE&) = delete; \
		TYPE& operator=(const TYPE&) = delete

#define API_SERIALIZABLE(TYPE, SUPER_T)\
	public: \
		typedef SUPER_T::SETTINGS_T SETTINGS_T; \
		typedef SUPER_T SUPER; \
		inline void Deserialize(istream& in, gE::Window* window) override { PlacementNew(*this, in, window); } \
		inline void Reload(gE::Window* window) override { GE_ASSERT(!IsFree()); SUPER_T tmp = move(*this); PlacementNew(*this, window, move(tmp)); } \
		TYPE() = default; \
		using SUPER_T::Free; \
		using SUPER_T::IsFree; \
		using SUPER_T::OnEditorGUI; \
		using SUPER_T::GetType; \
		using SUPER_T::SType; \
		TYPE(istream& in, gE::Window* window) \
		{ \
			SUPER_T tmp(in, window); \
			PlacementNew(*this, window, move(tmp)); \
			SUPER_T::Free(); \
		} \
		ALWAYS_INLINE const SUPER_T* operator->() const { return this; } \
		SUPER_T& GetSettings() { return *this; } \
		const SUPER_T& GetSettings() const { return *this; } \
		TYPE(gE::Window* window, const SUPER_T& settings) : TYPE(window, move(SUPER_T(settings))) { if(IsFree()) return; gE::Log::Write("POSSIBLE UNINTENTIONAL COPY: {}\n", PRETTY_FUNCTION); } \
		TYPE(gE::Window* window, SUPER_T&& INTERNAL_SETTINGS)

#define API_UNDERLYING_IMPL() \
	public: \
		inline Underlying* GetUnderlying() override { return this; } \
		inline const Underlying* GetUnderlying() const override { return this; }

#define API_SERIALIZABLE_IMPL_NAMESPACE(NAMESPACE, ...) \
	NAMESPACE##__VA_ARGS__::__VA_ARGS__(gE::Window* window, SUPER&& INTERNAL_SETTINGS) : SUPER(move(INTERNAL_SETTINGS))

#define API_SERIALIZABLE_IMPL(...) API_SERIALIZABLE_IMPL_NAMESPACE(, __VA_ARGS__)

#define API_REFLECTABLE_FACTORY_IMPL(TYPE, CONSTRUCTION_T, ...) \
	__VA_ARGS__ TYPE* TYPE::Factory(std::istream& in, TYPE::SETTINGS_T t) { return (TYPE*) new CONSTRUCTION_T(in, t); }
