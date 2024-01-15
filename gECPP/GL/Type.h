//
// Created by scion on 1/13/2024.
//

#pragma once

#include "GL/Binary/Binary.h"

#define GL_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL GLenum GLType<TYPE> = ENUM;

template<class T>
CONSTEXPR_GLOBAL GLenum GLType = 0;

GL_TYPE_(float, GL_FLOAT);
GL_TYPE_(u32, GL_UNSIGNED_INT);
GL_TYPE_(u8, GL_UNSIGNED_BYTE);
GL_TYPE_(i8, GL_BYTE);