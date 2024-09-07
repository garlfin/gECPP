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
#define API_ALIGN alignas(16)

#define TO_RAD 0.01745329251f
#define TO_DEG 57.2957795131f

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

enum class Dimension : u8
{
	D1D = 1,
	D2D = 2,
	D3D = 3
};

template<Dimension DIMENSION, typename INT_T = float>
using Position = std::conditional_t<DIMENSION == Dimension::D1D, INT_T, glm::vec<(u8) DIMENSION, INT_T, glm::defaultp>>;

template<Dimension DIMENSION, typename INT_T = u32>
using Size = Position<DIMENSION, INT_T>;

// World's dumbest optimization
constexpr u8 GLSizeOf(u32 t)
{
	if(t == GL_FLOAT) return 4;
	return 1 << ((t - 0x1400) >> 1); // i made this and i forgot how it worked
	// this is why you comment this kind of stupid stuff
}

namespace glm
{
	typedef quat quaternion;
}

typedef Size<Dimension::D1D> TextureSize1D;
typedef Size<Dimension::D2D> TextureSize2D;
typedef Size<Dimension::D3D> TextureSize3D;

enum class FOVType : u8
{
	Horizontal,
	Vertical
};

enum class AngleType : u8
{
	Degree,
	Radian
};

template<FOVType TO, AngleType UNIT = AngleType::Radian>
float constexpr fov_cast(float in, const TextureSize2D& size)
{
	float aspect;
	if constexpr(TO == FOVType::Horizontal) aspect = (float) size.x / size.y;
	else aspect = (float) size.y / size.x;

	if constexpr(UNIT == AngleType::Degree) in *= TO_RAD;
	float val = 2 * atanf(tanf(in / 2) * aspect);

	if constexpr(UNIT == AngleType::Degree) return val * TO_DEG;
	else return val;
}

inline void Decompose(const glm::mat4& m, glm::vec3& p, glm::quat& r, glm::vec3& s)
{
	p = m[3];

	s = glm::vec3
	{
		glm::length((glm::vec3) m[0]),
		glm::length((glm::vec3) m[1]),
		glm::length((glm::vec3) m[3])
	};

	glm::mat3 temp
	{
		m[0] / s.x,
		m[1] / s.y,
		m[2] / s.z
	};

	float root;

	r.w = glm::sqrt(m[0][0] + m[1][1] + m[2][2] + 1.0) * 0.5f;
	root = r.w * 4.0;
	r.x = (m[1][2] - m[2][1]) * root;
	r.y = (m[2][0] - m[0][2]) * root;
	r.z = (m[0][1] - m[1][0]) * root;
}