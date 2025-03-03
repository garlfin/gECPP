#include "Texture.h"

#include <GLAD/glad.h>
#include <Graphics/Texture/Texture.h>

#include "IMGUI/imgui.h"

namespace GL
{
	Texture::Texture(gE::Window* window, GLenum target, GPU::Texture& settings) :
		APIObject(window), _settings(&settings), _target(target)
	{
		glCreateTextures(target, 1, &ID);
		glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) _settings->Filter + (_settings->MipCount == 1 ? 0 : 0x102));
		glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) _settings->Filter);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_S, (GLint) _settings->WrapMode);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_T, (GLint) _settings->WrapMode);

		// if(WrapMode == WrapMode::Border)
		//	glTextureParameterfv(ID, GL_TEXTURE_BORDER_COLOR, WhiteBorderColor);

		if(target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_3D)
			glTextureParameteri(ID, GL_TEXTURE_WRAP_R, (GLint) _settings->WrapMode);
		else if (target == GL_TEXTURE_2D)
			glTextureParameteri(ID, GL_TEXTURE_MAX_ANISOTROPY, GE_ANISOTROPY_COUNT);
	}

	handle Texture::GetHandle() const
	{
		if(_handle)
			return _handle;
		if(!GLAD_GL_ARB_bindless_texture)
			return NullHandle;

		_handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(_handle);

		return _handle;
	}

	Texture::~Texture()
	{
		if(_handle) glMakeTextureHandleNonResidentARB(_handle);
		_handle = DEFAULT;
		glDeleteTextures(1, &ID);
	}

	API_SERIALIZABLE_IMPL(Texture2D), GL::Texture(window, GL_TEXTURE_2D, *this)
	{
		if(!MipCount) MipCount = GPU::GetMipCount<Dimension::D2D>(Size);
		glTextureStorage2D(ID, MipCount, Format, Size.x, Size.y);

		if(!Data) return;

		glm::u32vec2 size = Size;
		const u8* dataPtr = Data.Data.Data();

		for(u8 i = 0; i < Data.MipCount; i++, size >>= Size2D(1))
		{
			size = max(size, Size2D(1));
			u64 dataSize = Data.Scheme.Size<Dimension::D2D>(size);

			if(Data.Scheme.IsCompressed())
				glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Format, dataSize, dataPtr);
			else
				glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Data.PixelFormat, Data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Data.MipCount != MipCount) glGenerateTextureMipmap(ID);
	}

	void Texture2D::CopyFrom(const GL::Texture& o)
	{
		glCopyImageSubData(o.Get(), o.GetTarget(), 0, 0, 0, 0, ID, GL_TEXTURE_2D, 0, 0, 0, 0, GetSize().x, GetSize().y, 1);
	}

	REFLECTABLE_ONGUI_IMPL(Texture2D,
		OnEditorIcon(ImGui::GetContentRegionAvail().x);
	);

	REFLECTABLE_ICON_IMPL(Texture2D,
		ImGui::Image((ImTextureID) (API::Texture*) this, ImVec2(size, size));
	);

	API_SERIALIZABLE_IMPL(Texture3D), GL::Texture(window, GL_TEXTURE_3D, *this)
	{
		if(!MipCount) MipCount = GPU::GetMipCount<Dimension::D3D>(Size);
		glTextureStorage3D(ID, MipCount, Format, Size.x, Size.y, Size.z);

		if(!Data) return;

		glm::u32vec3 size = Size;
		const u8* dataPtr = Data.Data.Data();

		for(u8 i = 0; i < Data.MipCount; i++, size >>= Size3D(1))
		{
			size = max(size, Size3D(1));
			u64 dataSize = Data.Scheme.Size<Dimension::D3D>(size);

			if(Data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Data.PixelFormat, Data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Data.MipCount != MipCount) glGenerateTextureMipmap(ID);
	}

	void Texture3D::CopyFrom(const GL::Texture& o)
	{
		glCopyImageSubData(o.Get(), o.GetTarget(), 0, 0, 0, 0, ID, GL_TEXTURE_3D, 0, 0, 0, 0, GetSize().x, GetSize().y, GetSize().z);
	}

	API_SERIALIZABLE_IMPL(TextureCube), GL::Texture(window, GL_TEXTURE_CUBE_MAP, *this)
	{
		if(!MipCount) MipCount = GPU::GetMipCount<Dimension::D1D>(Size);
		glTextureStorage2D(ID, MipCount, Format, Size, Size);

		if(!Data.Data) return;

		u32 size = Size;
		u8* dataPtr = Data.Data.Data(); // sobbing rn

		for(u8 i = 0; i < Data.MipCount; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = Data.Scheme.Size<Dimension::D3D>(Size3D(size, size, 6));

			if(Data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, Data.PixelFormat, Data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Data.MipCount != MipCount) glGenerateTextureMipmap(ID);
	}

	void TextureCube::CopyFrom(const GL::Texture& o)
	{
		// TODO
	}

	API_SERIALIZABLE_IMPL(Texture1D), GL::Texture(window, GL_TEXTURE_1D, *this)
	{
		if(!MipCount) MipCount = GPU::GetMipCount<Dimension::D1D>(Size);
		glTextureStorage1D(ID, MipCount, Format, Size);

		if(!Data.Data) return;

		u32 size = Size;
		const u8* dataPtr = Data.Data.Data(); // sobbing rn

		for(u8 i = 0; i < Data.MipCount; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = Data.Scheme.Size<Dimension::D1D>(size);

			if(Data.Scheme.IsCompressed())
				glCompressedTextureSubImage1D(ID, i, 0, size, Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, Data.PixelFormat, Data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Data.MipCount != MipCount) glGenerateTextureMipmap(ID);
	}

	void Texture1D::CopyFrom(const GL::Texture& o)
	{
		// TODO
	}
}
