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

	API::Texture2D* Create2D(Window*, const Header&, Array<std::byte>&&, GPU::WrapMode, GPU::FilterMode);
	API::Texture3D* Create3D(Window*, const Header&, Array<std::byte>&&, GPU::WrapMode, GPU::FilterMode);
	API::TextureCube* CreateCube(Window*, const Header&, Array<std::byte>&&, GPU::WrapMode, GPU::FilterMode);

	API::Texture* Read(Window* window, const Path& path, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		Header header;
		Array<std::byte> imageData = ReadRaw(path, header);

		if(!imageData)
			return nullptr;

		if(header.Faces == 6)
			return CreateCube(window, header, std::move(imageData), wrapMode, filterMode);
		if(header.Size.z == 1)
			return Create2D(window, header, std::move(imageData), wrapMode, filterMode);
		return Create3D(window, header, std::move(imageData), wrapMode, filterMode);
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
		Size = ::Read<u32vec3>(in);
		Size = { Size.y, Size.x, Size.z }; // NOLINT
		// they store it height, width 🤦‍♂️
		Surfaces = ::Read<u32>(in);
		Faces = ::Read<u32>(in);
		MipCount = ::Read<u32>(in);
		in.seekg(::Read<u32>(in), std::ios::cur); // I couldn't give two hoots about the metadata
	}

	void Header::ISerialize(ostream&) const { }

	API::Texture2D* Create2D(Window* window, const Header& header, Array<std::byte>&& imageData, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
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

		return new API::Texture2D(window, move(settings));
	}

	API::Texture3D* Create3D(Window* window, const Header& header, Array<std::byte>&& imageData, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		GE_ASSERTM(header.Faces == 1, "3D Cubemap?");
		GE_ASSERT(header.Format == PixelFormat::RGBU8);

		GPU::Texture3D settings;
		GPU::TextureData data;

		data.PixelFormat = GL_RGB;
		data.PixelType = GL_UNSIGNED_BYTE;
		data.Scheme = GPU::CompressionScheme(1, 3);
		data.MipCount = 1;
		data.Data = move(imageData);

		settings.Format = GL_RGB8;
		settings.WrapMode = wrapMode;
		settings.Filter = filterMode;
		settings.MipCount = 1;
		settings.Data = move(data);
		settings.Size = header.Size;

		return new API::Texture3D(window, move(settings));
	}

	API::TextureCube* CreateCube(Window* window, const Header& header, Array<std::byte>&& imageData, GPU::WrapMode wrapMode, GPU::FilterMode filterMode)
	{
		GE_ASSERTM(header.Size.x == header.Size.y, "Cubemap not square!");
		GE_ASSERT(header.Format == PixelFormat::RGB16F);

		GPU::TextureCube settings;
		GPU::TextureData data;

		data.PixelFormat = GL_RGB;
		data.PixelType = GL_HALF_FLOAT;
		data.Scheme = GPU::CompressionScheme(1, 6);
		data.MipCount = 1;
		data.Data = move(imageData);

		settings.Format = GL_RGB16F;
		settings.WrapMode = wrapMode;
		settings.Filter = filterMode;
		settings.MipCount = 1;
		settings.Data = move(data);
		settings.Size = header.Size.x;

		return new API::TextureCube(window, move(settings));
	}
}
