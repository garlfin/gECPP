//
// Created by scion on 3/1/2025.
//

#pragma once

#include <Core/Macro.h>
#include <Graphics/Texture/Texture.h>

namespace gE::PVR
{
    enum class Flags : u32
    {
        None = 0,
        PreMultiplied = 2
    };

    // TODO: This is actually wrong!
    enum class PixelFormat : u64
    {
        DXT1 = 7,
        DXT3 = 9,
        DXT5 = 11,
        BC5 = 13,
        Depth,
        RGB8 = 2260630272894834,
        RGBA8 = 7267626108080808,
        RGB32F = 9042521072232306,
        RGB16F = 4521260539340658
    };

    enum class ColorSpace : u32
    {
        Linear = 0,
        SRGB = 1,
    };

    struct Header : Serializable<>
    {
    public:
        explicit Header(istream& in, SETTINGS_T s) : Serializable(in, s) { IDeserialize(in, s); }
        Header() = default;
        inline void Deserialize(istream& in, SETTINGS_T s) override { PlacementNew(*this, in, s); }
        inline void Serialize(ostream& out) const override { ISerialize(out); }

        uint32_t Version;
        Flags Flags;
        PixelFormat Format;
        ColorSpace ColorSpace;
        glm::u32vec2 Size;
        uint32_t Depth;
        uint32_t Surfaces;
        uint32_t Faces;
        uint32_t MipCount;

    private:
        void IDeserialize(istream& in, SETTINGS_T s);
        void ISerialize(ostream& out) const;
    };

    constexpr GLenum PVRToInternalFormat(PixelFormat f)
    {
        switch(f)
        {
        case PixelFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case PixelFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case PixelFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case PixelFormat::BC5: return GL_COMPRESSED_RG_RGTC2;
        case PixelFormat::Depth: return GL_DEPTH_COMPONENT16;
        case PixelFormat::RGB32F: return GL_RGB32F;
        case PixelFormat::RGB16F: return GL_RGB16F;
        default: return GL_RGB8;
        }
    }
    NODISCARD API::Texture* Read(Window*, const Path&, GPU::WrapMode = GPU::WrapMode::Repeat, GPU::FilterMode = GPU::FilterMode::Linear);
    NODISCARD File ReadAsFile(Window*, const Path&, GPU::WrapMode = GPU::WrapMode::Repeat, GPU::FilterMode = GPU::FilterMode::Linear);
    NODISCARD Array<std::byte> ReadRaw(const Path& path, Header& header);
}