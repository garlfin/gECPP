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
		SUPER tmp(in, nullptr); \
		new(this) TYPE(window, move(tmp)); \
		SUPER::Free(); \
	} \
	TYPE(gE::Window* window, const SUPER& settings) : TYPE(window, (SUPER&&) SUPER(settings)) {} \
	TYPE(gE::Window* window, SUPER&& INTERNAL_SETTINGS) \
	GET_CONST(SUPER_T&, Settings, *this)

#define API_DEFAULT_CM_CONSTRUCTOR(TYPE) \
	public: \
	TYPE(TYPE&&) = default; \
	TYPE& operator=(TYPE&&) = default; \
	TYPE(const TYPE&) = delete; \
	TYPE& operator=(const TYPE&) = delete

