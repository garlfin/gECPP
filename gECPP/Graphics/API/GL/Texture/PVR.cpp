//
// Created by scion on 10/25/2023.
//

#include <fstream>
#include "Texture.h"

namespace PVR
{
	Array<u8> Read(const char* file, Header& header)
	{
		std::ifstream src;
		src.open(file, std::ios::in | std::ios::binary);

		if(!file) return {};

		src.seekg(0, std::ios::end);
		size_t copySize = src.tellg();
		src.seekg(0, std::ios::beg);

		header.Serialize(src);

		copySize -= src.tellg();
		Array<u8> data(copySize);
		src.read((char*) data.Data(), copySize);

		return data;
	}

	API::Texture* Read(gE::Window* window, const char* path, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		Header header;
		Array<u8> imageData = Read(path, header);
		if(!imageData) return nullptr;

		API::Texture* tex = nullptr;

		if(header.Faces == 1)
		{
			GPU::TextureSettings2D settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				(u8) header.MipCount,
				header.Size,
			};

			GPU::TextureData data
			{
				GL_NONE,
				GL_NONE,
				GPU::CompressionScheme(4, 16), // 16 bytes per 4x4 block
				(u8) header.MipCount,
				std::move(imageData)
			};

			tex = new API::Texture2D(window, settings, std::move(data));
		}
		else if(header.Faces == 6)
		{
			GE_ASSERT(header.Size.x == header.Size.y, "Cubemap not square!");

			GPU::TextureSettings1D settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				(u8) header.MipCount,
				header.Size.x,
			};

			GPU::TextureData data
			{
				GL_RGB,
				GL_HALF_FLOAT,
				GPU::CompressionScheme(1, 6), // 6 bytes per pixel
				(u8) header.MipCount,
				std::move(imageData)
			};

			tex = new API::TextureCube(window, settings, std::move(data));
		}
		else LOG("Unsupported texture format!");

		return tex;
	}

	void Header::ISerialize(istream& src)
	{
		Version = ::Read<u32>(src);
		Flags = ::Read<PVR::Flags>(src);
		Format = ::Read<PixelFormat>(src);
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