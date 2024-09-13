#pragma once

#include <Graphics/Graphics.h>
#include <Engine/Binary/Binary.h>

#define GL_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL GLenum GLType<TYPE> = ENUM;

template<class T>
CONSTEXPR_GLOBAL GLenum GLType = 0;

GL_TYPE_(float, GL_FLOAT);
GL_TYPE_(u32, GL_UNSIGNED_INT);
GL_TYPE_(u8, GL_UNSIGNED_BYTE);
GL_TYPE_(i8, GL_BYTE);

namespace GL
{
	class APIObject : public GPU::APIObject<u32>
	{
	public:
		typedef GPU::APIObject<u32> SUPER;

		using SUPER::APIObject;
		using SUPER::operator=;
	};

	typedef APIObject GLObject;
}