//
// Created by scion on 9/5/2024.
//

#pragma once

#define API_SERIALIZABLE(TYPE, SPR) \
	public: \
		typedef SPR::SETTINGS_T SETTINGS_T; \
		typedef SPR SUPER; \
		ALWAYS_INLINE void Serialize(istream& in, gE::Window* window) override { TYPE(in, window); } \
		TYPE(istream& in, gE::Window* window) : API::APIObject(window) \
		{ \
			this->~TYPE(); \
			SUPER::Serialize(in, window); \
			TYPE(window, std::move(*this)); \
			SUPER::Free(); \
		} \
		TYPE(gE::Window* window, const SUPER& settings)
