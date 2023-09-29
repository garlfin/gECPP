//
// Created by scion on 8/8/2023.
//

#pragma once

#include "GLM/vec2.hpp"
#include "GLM/vec3.hpp"
#include "GLM/vec4.hpp"
#include "GLM/gtx/quaternion.hpp"
#include "GLM/mat3x3.hpp"
#include "GLM/mat4x4.hpp"
#include "GLAD/types.h"

#include <cstdint>

#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define TO_RAD 0.01745329251f
#define TO_DEG 57.2957795131f

typedef uint8_t ubyte;
typedef uint8_t u8;
typedef int8_t sbyte;
typedef int8_t i8;

typedef uint16_t ushort;
typedef uint16_t u16;
typedef int16_t sshort;
typedef int16_t i16;

typedef uint32_t uint;
typedef uint32_t u32;
typedef int32_t sint;
typedef int32_t i32;

typedef uint64_t ulong;
typedef uint64_t u64;
typedef int64_t slong;
typedef int64_t i64;

// World's dumbest optimization
constexpr u8 GetSizeOfGLType(u32 t)
{
	if(t == GL_FLOAT) return 4;
	return 1 << ((t - 0x1400) >> 1);
}

namespace glm
{
	typedef quat quaternion;
}

namespace GL
{
	typedef glm::u32vec2 TextureSize2D;
	typedef glm::u32vec3 TextureSize3D;
	typedef u32 TextureSize1D;
}

enum class FOVType : u8
{
	Horizontal, Vertical
};

enum class AngleType : u8
{
	Degree, Radian
};

template<FOVType TO, AngleType UNIT = AngleType::Radian>
float fov_cast(float in, const GL::TextureSize2D& size)
{
	float aspect;
	if constexpr (TO == FOVType::Horizontal) aspect = (float) size.x / size.y;
	else aspect = (float) size.y / size.x;

	if constexpr (UNIT == AngleType::Degree) in *= TO_RAD;
	float val = 2 * atanf(tanf(in / 2) * aspect);

	if constexpr (UNIT == AngleType::Degree) return val * TO_DEG;
	else return val;
}

template<AngleType TO, typename T>
inline constexpr T degree_cast(const T& t)
{
	if constexpr(TO == AngleType::Radian) return t * TO_RAD;
	else return t * TO_DEG;
}



