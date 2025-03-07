//
// Created by scion on 10/25/2023.
//

#include "PVR.h"
#include <fstream>

namespace gE::PVR
{
	Array<std::byte> ReadRaw(const Path& path, Header& header)
	{
		std::ifstream src;
		src.open(path, std::ios::in | std::ios::binary);

		if(!src.is_open()) Log::Write("ERROR: COULD NOT OPEN FILE {}", path.string());

		src.seekg(0, std::ios::end);
		size_t copySize = src.tellg();
		src.seekg(0, std::ios::beg);

		header.Deserialize(src, nullptr);

		copySize -= src.tellg();
		Array<std::byte> data(copySize);
		src.read((char*) data.Data(), copySize);

		return data;
	}

	API::Texture* Read(Window* window, const Path& path, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		Header header;
		Array<std::byte> imageData = ReadRaw(path, header);
		if(!imageData) return nullptr;

		API::Texture* tex = nullptr;

		if(header.Faces == 1)
		{
			GPU::Texture2D settings;
			GPU::TextureData data;

			data.PixelFormat = GL_NONE;
			data.PixelType = GL_NONE;
			data.Scheme = GPU::CompressionScheme(4, 16);
			data.MipCount = 1;
			data.Data = move(imageData);

			settings.Format = PVRToInternalFormat(header.Format);
			settings.WrapMode = wrapMode;
			settings.Filter = filterMode;
			settings.MipCount = 1;
			settings.Data = move(data);
			settings.Size = header.Size;

			tex = new API::Texture2D(window, move(settings));
		}
		else if(header.Faces == 6)
		{
			GE_ASSERTM(header.Size.x == header.Size.y, "Cubemap not square!");

			GPU::TextureCube settings;
			GPU::TextureData data;

			data.PixelFormat = GL_RGB;
			data.PixelType = GL_HALF_FLOAT;
			data.Scheme = GPU::CompressionScheme(1, 6);
			data.MipCount = 1;
			data.Data = move(imageData);

			settings.Format = PVRToInternalFormat(header.Format);
			settings.WrapMode = wrapMode;
			settings.Filter = filterMode;
			settings.MipCount = 1;
			settings.Data = move(data);
			settings.Size = header.Size.x;

			tex = new API::TextureCube(window, move(settings));
		}
		else
			Log::Write("Unsupported texture format!");

		return tex;
	}

	File ReadAsFile(Window* window, const Path& path, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		return File(window, path, ref_cast(&Read(window, path, wrapMode, filterMode)->GetSettings()));
	}

	void Header::IDeserialize(istream& in, SETTINGS_T)
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

	void Header::ISerialize(ostream&) const { }
}
