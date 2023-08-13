//
// Created by scion on 8/8/2023.
//

#pragma once

#include <MathFU/vector.h>
#include <MathFU/matrix.h>
#include <GLAD/types.h>

#include <cstdint>

#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define VEC_ALIAS(TYPE, NAME) \
	typedef MathFU::Vector<TYPE, 2> NAME##vec2; \
	typedef MathFU::Vector<TYPE, 3> NAME##vec3; \
	typedef MathFU::Vector<TYPE, 4> NAME##vec4; \
	typedef MathFU::Vector<TYPE, 2> p##NAME##vec2; \
	typedef MathFU::Vector<TYPE, 3> p##NAME##vec3; \
	typedef MathFU::Vector<TYPE, 4> p##NAME##vec4;

namespace gl
{
	VEC_ALIAS(float, );

	VEC_ALIAS(uint8_t, u8);
	VEC_ALIAS(int8_t, i8);

	VEC_ALIAS(uint16_t, u16);
	VEC_ALIAS(int16_t, i16);

	VEC_ALIAS(uint32_t, u32);
	VEC_ALIAS(int32_t, i32);
	VEC_ALIAS(uint32_t, u);
	VEC_ALIAS(int32_t, i);

	VEC_ALIAS(uint64_t, u64);
	VEC_ALIAS(int64_t, i64);

	typedef MathFU::Matrix<float, 4, 4> mat4;
	typedef MathFU::Matrix<float, 3, 3> mat3;
}

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
