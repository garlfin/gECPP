//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "TextureSettings.h"

namespace GL
{
	class Texture;
}

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size)
	{
		u32 largest;

		if constexpr(T == Dimension::D1D) largest = size;
		else if constexpr(T == Dimension::D2D) largest = glm::max(size.x, size.y);
		else largest = glm::max(size.x, glm::max(size.y, size.z));

		return 32 - __builtin_clz(largest);
	}

	class Texture : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(Texture, Serializable<gE::Window*>);

	 public:
		Texture(ITextureSettings&, TextureData&&);

		GET_CONST(const ITextureSettings&, Settings, Settings);
		GET_CONST(const TextureData&, Data, _data);

		ALWAYS_INLINE void Free() override { _data.Free(); }

	 protected:
		ITextureSettings Settings;

	 private:
		TextureData _data;
	};

	class Texture1D : public Texture
	{
		SERIALIZABLE_PROTO_T_CONSTRUCTABLE(Texture1D, Texture);
	public:
		Texture1D(TextureSettings1D& s, TextureData&& d) : Texture(s, std::move(d)), _size(s.Size) { Construct(); }
		NODISCARD ALWAYS_INLINE TextureSize1D GetSize(u8 mip = 0) const { return glm::max(_size >> (u32) mip, 1u); }

	private:
		ALWAYS_INLINE void Construct() { if(!Settings.MipCount) Settings.MipCount = GetMipCount<Dimension::D1D>(_size); }
		TextureSize1D _size;
	};

 	class Texture2D : public Texture
	{
		SERIALIZABLE_PROTO_T_CONSTRUCTABLE(Texture2D, Texture);
	 public:
 		Texture2D(TextureSettings2D& s, TextureData&& d) : Texture(s, std::move(d)), _size(s.Size) { Construct(); }
 		NODISCARD ALWAYS_INLINE TextureSize2D GetSize(u8 mip = 0) const { return max(_size >> glm::u32vec2(mip), glm::u32vec2(1)); }

	 private:
 		ALWAYS_INLINE void Construct() { if(!Settings.MipCount) Settings.MipCount = GetMipCount<Dimension::D2D>(_size); }
		TextureSize2D _size;
	};

	class Texture3D : public Texture
	{
		SERIALIZABLE_PROTO_T_CONSTRUCTABLE(Texture3D, Texture);
	public:
		Texture3D(TextureSettings3D& s, TextureData&& d) : Texture(s, std::move(d)), _size(s.Size) { Construct(); }
		NODISCARD ALWAYS_INLINE TextureSize3D GetSize(u8 mip = 0) const { return max(_size >> glm::u32vec3(mip), glm::u32vec3(1)); }

	private:
		ALWAYS_INLINE void Construct() { if(!Settings.MipCount) Settings.MipCount = GetMipCount<Dimension::D3D>(_size); }
		TextureSize3D _size;
	};

	class TextureCube : public Texture
	{
		SERIALIZABLE_PROTO_T_CONSTRUCTABLE(TextureCube, Texture);
	public:
		TextureCube(TextureSettings1D& s, TextureData&& d) : Texture(s, std::move(d)), _size(s.Size) { Construct(); }
		NODISCARD ALWAYS_INLINE TextureSize1D GetSize(u8 mip = 0) const { return MAX(_size >> mip, 1); }

	private:
		ALWAYS_INLINE void Construct() { if(!Settings.MipCount) Settings.MipCount = GetMipCount<Dimension::D1D>(_size); }
		TextureSize1D _size;
	};
}

#include "Texture.inl"

#include <Graphics/API/GL/Texture/Texture.h>

namespace PVR
{
	NODISCARD API::Texture* Read(gE::Window* window, const char* path, GPU::WrapMode = GPU::WrapMode::Repeat, GPU::FilterMode = GPU::FilterMode::Linear);
	NODISCARD Array<u8> Read(const char* path, Header& header);
}