//
// Created by scion on 8/7/2024.
//

#pragma once

namespace GL
{
	template<class T, bool DYNAMIC>
	inline void Buffer<T, DYNAMIC>::ISerialize(std::istream&, SETTINGS_T) { }


	template<class T, bool DYNAMIC>
	inline void Buffer<T, DYNAMIC>::IDeserialize(std::ostream&) const { }

}