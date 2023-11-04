//
// Created by scion on 10/25/2023.
//

#include "Texture.h"

namespace PVR
{
	u8* Read(const char* path, PVR::Header& header)
	{

		u32 fileLen;
		u8* file = ReadFile(path, fileLen, false);
		u8* textureData = file;

		if(!file) return nullptr;

		header.Serialize(textureData);
		size_t copySize = fileLen - (textureData - file);
		u8* textureDataCopy = (u8*) memcpy(new u8[copySize], textureData, copySize);

		delete[] file;
		return textureDataCopy;
	}

	GL::Texture* Read(gE::Window* window, const char* path, GL::WrapMode wrapMode, GL::FilterMode filterMode)
	{

		Header header;
		u8* imageData = Read(path, header);
		if(!imageData) return nullptr;

		GE_ASSERT(header.Depth + header.Surfaces + header.Faces == 3, "Unexpected Texture Format");

		GL::TextureSettings<GL::TextureDimension::D2D> settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				(u8) header.MipCount,
				header.Size,
			};

		GL::TextureData data
			{
				GL_NONE,
				GL_NONE,
				GL::CompressionScheme(4, 16),
				imageData,
				true
			};

		auto* tex = new GL::Texture2D(window, settings, data);
		delete[] imageData;

		return tex;
	}

	void Header::Serialize(u8*& ptr)
	{

		Version = ::Read < u32 > (ptr);
		Flags = ::Read < PVR::Flags > (ptr);
		Format = ::Read < PVR::PixelFormat > (ptr);
		ColorSpace = ::Read < PVR::ColorSpace > (ptr);
		::Read < uint32_t > (ptr); // This was like bpc or something; unimportant w/ compression
		Size = ::Read < glm::u32vec2 > (ptr);
		Size = { Size.y, Size.x }; // NOLINT
		// they store it height, width 🤦‍♂️
		Depth = ::Read < u32 > (ptr);
		Surfaces = ::Read < u32 > (ptr);
		Faces = ::Read < u32 > (ptr);
		MipCount = ::Read < u32 > (ptr);
		ptr += ::Read < u32 > (ptr); // I couldn't give two hoots about the metadata
	}

	void Header::Deserialize(gETF::SerializationBuffer&) const
	{ }
}