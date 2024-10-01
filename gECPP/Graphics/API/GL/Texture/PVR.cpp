//
// Created by scion on 10/25/2023.
//

#include <fstream>
#include "Texture.h"

namespace PVR
{
	Array<u8> Read(const Path& path, Header& header)
	{
		std::ifstream src;
		src.open(path, std::ios::in | std::ios::binary);

		if(!src.is_open()) LOG("ERROR: COULD NOT OPEN FILE " << path);

		src.seekg(0, std::ios::end);
		size_t copySize = src.tellg();
		src.seekg(0, std::ios::beg);

		header.Serialize(src, nullptr);

		copySize -= src.tellg();
		Array<u8> data(copySize);
		src.read((char*) data.Data(), copySize);

		return data;
	}

	GL::Texture* Read(gE::Window* window, const Path& path, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		Header header;
		Array<u8> imageData = Read(path, header);
		if(!imageData) return nullptr;

		GL::Texture* tex = nullptr;

		if(header.Faces == 1)
		{
			GPU::TextureSettings2D settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				0,
				header.Size,
			};

			GPU::TextureData data
			{
				GL_NONE,
				GL_NONE,
				GPU::CompressionScheme(4, 16), // 16 bytes per 4x4 block
				1,
				std::move(imageData)
			};

			tex = new GL::Texture2D(window, settings, std::move(data));
		}
		else if(header.Faces == 6)
		{
			GE_ASSERT(header.Size.x == header.Size.y, "Cubemap not square!");

			GPU::TextureSettings1D settings
			{
				PVRToInternalFormat(header.Format),
				wrapMode,
				filterMode,
				0,
				header.Size.x,
			};

			GPU::TextureData data
			{
				GL_RGB,
				GL_HALF_FLOAT,
				GPU::CompressionScheme(1, 6), // 6 bytes per pixel
				1,
				std::move(imageData)
			};

			tex = new GL::TextureCube(window, settings, std::move(data));
		}
		else
			LOG("Unsupported texture format!");

		return tex;
	}

	void Header::ISerialize(istream& in, SETTINGS_T)
	{
		Version = ::Read<u32>(in);
		Flags = ::Read<PVR::Flags>(in);
		Format = ::Read<PixelFormat>(in);
		ColorSpace = ::Read<PVR::ColorSpace>(in);
		::Read<uint32_t>(in); // This was like bpc or something; unimportant w/ compression
		Size = ::Read<glm::u32vec2>(in);
		Size = { Size.y, Size.x }; // NOLINT
		// they store it height, width 🤦‍♂️
		Depth = ::Read<u32>(in);
		Surfaces = ::Read<u32>(in);
		Faces = ::Read<u32>(in);
		MipCount = ::Read<u32>(in);
		in.seekg(::Read<u32>(in), std::ios::cur); // I couldn't give two hoots about the metadata
	}

	void Header::IDeserialize(ostream&) const { }
}