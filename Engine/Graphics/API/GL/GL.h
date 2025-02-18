#pragma once

#include <Core/Serializable/Macro.h>
#include <GLAD/glad.h>
#include <Graphics/Graphics.h>

#define GL_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL GLenum GLType<TYPE> = ENUM;

template<class T>
CONSTEXPR_GLOBAL GLenum GLType = 0;

GL_TYPE_(float, GL_FLOAT);
GL_TYPE_(u32, GL_UNSIGNED_INT);
GL_TYPE_(i32, GL_INT);
GL_TYPE_(u16, GL_UNSIGNED_SHORT);
GL_TYPE_(i16, GL_SHORT);
GL_TYPE_(u8, GL_UNSIGNED_BYTE);
GL_TYPE_(i8, GL_BYTE);

namespace GL
{
	class APIObject : public GPU::APIObject<u32>
	{
		API_DEFAULT_CM_CONSTRUCTOR(APIObject);

	public:
		typedef GPU::APIObject<u32> SUPER;

		using SUPER::APIObject;
		using SUPER::operator=;
	};

	typedef APIObject GLObject;
}