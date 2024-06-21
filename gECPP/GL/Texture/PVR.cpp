//
// Created by scion on 10/25/2023.
//

#include <fstream>
#include "Texture.h"

namespace PVR
{
	u8* Read(const char* file, PVR::Header& header)
	{
		std::ifstream src;
		src.open(file, std::ios::in | std::ios::binary);

		if(!file) return nullptr;

		src.seekg(0, std::ios::end);
		size_t copySize = src.tellg();
		src.seekg(0, std::ios::beg);

		header.Serialize(src);

		copySize -= src.tellg();
		u8* data = new u8[copySize];
		src.read((char*) data, copySize);

		return data;
	}

	GL::Texture* Read(gE::Window* window, const char* path, GL::WrapMode wrapMode, GL::FilterMode filterMode)
	{
		Header header;
		u8* imageData = Read(path, header);
		if(!imageData) return nullptr;

		GL::Texture* tex = nullptr;

		if(header.Faces == 1)
		{
			GL::TextureSettings<GL::TextureDimension::D2D> settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				(u8) header.MipCount,
				0,
				header.Size,
			};

			GL::TextureData data
			{
				GL_NONE,
				GL_NONE,
				GL::CompressionScheme(4, 16), // 16 bytes per 4x4 block
				imageData,
				true
			};

			tex = new GL::Texture2D(window, settings, data);
		}
		else if(header.Faces == 6)
		{
			GE_ASSERT(header.Size.x == header.Size.y, "Cubemap not square!");

			GL::TextureSettings<GL::TextureDimension::D1D> settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				(u8) header.MipCount,
				0,
				header.Size.x,
			};

			GL::TextureData data
			{
				GL_RGB,
				GL_HALF_FLOAT,
				GL::CompressionScheme(1, 6), // 6 bytes per pixel
				imageData,
				true
			};

			tex = new GL::TextureCube(window, settings, data);
		}
		else LOG("Unsupported texture format!");

		delete[] imageData;
		return tex;
	}

	void Header::ISerialize(istream& src)
	{
		Version = ::Read<u32>(src);
		Flags = ::Read<PVR::Flags>(src);
		Format = ::Read<PVR::PixelFormat>(src);
		ColorSpace=::Read<PVR::ColorSpace>(src);
		::Read<uint32_t>(src); // This was like bpc or something; unimportant w/ compression
		Size = ::Read<glm::u32vec2>(src);
		Size = { Size.y, Size.x }; // NOLINT
		// they store it height, width 🤦‍♂️
		Depth = ::Read<u32>(src);
		Surfaces = ::Read<u32>(src);
		Faces = ::Read<u32>(src);
		MipCount = ::Read<u32>(src);
		src.seekg(::Read<u32>(src), std::ios::cur); // I couldn't give two hoots about the metadata
	}

	void Header::IDeserialize(ostream&) const { }
}