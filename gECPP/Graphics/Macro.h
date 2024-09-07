//
// Created by scion on 9/5/2024.
//

#pragma once

#define API_SERIALIZABLE(TYPE, SUPER) \
	public: \
		ALWAYS_INLINE void Serialize(istream& in, SETTINGS_T s) override { TYPE(in, s); } \
		inline void Deserialize(ostream& out) const override { SUPER::Deserialize(out); TYPE::IDeserialize(out); } \
	private: \
		void ISerialize(istream& in, SETTINGS_T s); \
		void IDeserialize(ostream& out) const; \
	public: \
		explicit TYPE(istream& in, SETTINGS_T s)
