//
// Created by scion on 9/5/2024.
//

#pragma once

#define API_SERIALIZABLE_INIT(TYPE, SUPER_T, ...)\
	public: \
	typedef SUPER_T::SETTINGS_T SETTINGS_T; \
	typedef SUPER_T SUPER; \
	ALWAYS_INLINE void Serialize(istream& in, gE::Window* window) override { TYPE(in, window); } \
	TYPE(istream& in, gE::Window* window) : __VA_ARGS__ \
	{ \
		this->~TYPE(); \
		SUPER::Serialize(in, window); \
		TYPE(window, std::move(*this)); \
		SUPER::Free(); \
	} \
	TYPE(gE::Window* window, const SUPER& settings)

#define API_SERIALIZABLE(TYPE, SUPER_T) \
	API_SERIALIZABLE_INIT(TYPE, SUPER_T, API::APIObject(window))

