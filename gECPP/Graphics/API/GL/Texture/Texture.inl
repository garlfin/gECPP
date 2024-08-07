//
// Created by scion on 8/7/2024.
//

#pragma once

namespace GL
{
	inline void Texture2D::ISerialize(istream&, SETTINGS_T) { }
	inline void Texture2D::IDeserialize(ostream&) const { }

	inline void Texture3D::ISerialize(istream&, SETTINGS_T) { }
	inline void Texture3D::IDeserialize(ostream&) const { }

	inline void TextureCube::ISerialize(istream&, SETTINGS_T) { }
	inline void TextureCube::IDeserialize(ostream&) const { }
}