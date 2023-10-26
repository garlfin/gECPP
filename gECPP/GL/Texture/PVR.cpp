//
// Created by scion on 10/25/2023.
//

#include "Texture.h"

namespace PVR
{
	GL::Texture2D* Read(gE::Window* window, const char* path, GL::WrapMode wrapMode, GL::FilterMode filterMode)
	{
		u32 fileLen = 0;
		u8* f = ReadFile(path, fileLen, false);
		if (!f) return nullptr;

		u8* ptr = f;
		PVRHeader h;
		h.Serialize(ptr);

		if (h.Depth + h.Surfaces + h.Faces > 3) std::cout << "Unexpected 3D Texture" << std::endl;

		GL::TextureSettings<GL::TextureDimension::D2D> settings
		{
			PVRToInternalFormat(h.Format),
			wrapMode,
			filterMode,
			(u8) h.MipCount,
			h.Size,
		};

		GL::TextureData data
		{
			GL_NONE,
			GL_NONE,
			GL::CompressionScheme(16, 16),
			ptr,
			true
		};

		auto* tex = new GL::Texture2D(window, settings, data);

		delete[] f;
		return tex;
	}

	void PVRHeader::Serialize(u8*& ptr)
	{
		Version = ::Read<u32>(ptr);
		Flags = ::Read<PVRFlags>(ptr);
		Format = ::Read<PVRPixelFormat>(ptr);
		ColorSpace = ::Read<PVRColorSpace>(ptr);
		::Read<uint32_t>(ptr); // This was like bpc or something; unimportant w/ compression
		Size = ::Read<glm::u32vec2>(ptr);
		Size = { Size.y, Size.x }; // NOLINT
		// they store it height, width 🤦‍♂️
		Depth = ::Read<u32>(ptr);
		Surfaces = ::Read<u32>(ptr);
		Faces = ::Read<u32>(ptr);
		MipCount = ::Read<u32>(ptr);
		ptr += ::Read<u32>(ptr); // I couldn't give two hoots about the metadata
	}

	void PVRHeader::Deserialize(gETF::SerializationBuffer&) const
	{}
}