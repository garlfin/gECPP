//
// Created by scion on 9/5/2024.
//

#pragma once

#define API_SERIALIZABLE_INIT(TYPE, SUPER_T, ...)\
	public: \
	typedef SUPER_T::SETTINGS_T SETTINGS_T; \
	typedef SUPER_T SUPER; \
	ALWAYS_INLINE void Serialize(istream& in, gE::Window* window) override { TYPE(in, window); } \
	TYPE() = default; \
	TYPE(istream& in, gE::Window* window) : __VA_ARGS__ \
	{ \
		this->~TYPE(); \
		SUPER::Serialize(in, window); \
		TYPE(window, std::move(*this)); \
		SUPER::Free(); \
	} \
	TYPE(gE::Window* window, const SUPER& settings) : TYPE(window, (SUPER&&) SUPER(settings)) {} \
	TYPE(gE::Window* window, SUPER&& settings)

#define API_SERIALIZABLE(TYPE, SUPER_T) \
	API_SERIALIZABLE_INIT(TYPE, SUPER_T, API::APIObject(window))

#define API_DEFAULT_CM_CONSTRUCTOR(TYPE) \
	TYPE(TYPE&&) noexcept = default; \
	TYPE(const TYPE&) = delete; \
	TYPE& operator=(TYPE&&) noexcept = default; \
	TYPE& operator=(const TYPE&) = delete

