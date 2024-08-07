//
// Created by scion on 8/7/2024.
//

#pragma once

#define TEX_SERIALIZABLE(TYPE, FORMAT) \
    inline void TYPE::ISerialize(istream& in, SETTINGS_T s) { _size = Read<FORMAT>(in); } \
    inline void TYPE::IDeserialize(ostream& out) const { Write(out, _size); }

namespace GPU
{
    inline void Texture::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Settings);
        Read(in, _data);
    }

    inline void Texture::IDeserialize(ostream& out) const
    {
        Write(out, Settings);
        Write(out, _data);
    }

    TEX_SERIALIZABLE(Texture1D, TextureSize1D);
    TEX_SERIALIZABLE(Texture2D, TextureSize2D);
    TEX_SERIALIZABLE(Texture3D, TextureSize3D);
    TEX_SERIALIZABLE(TextureCube, TextureSize1D);
}