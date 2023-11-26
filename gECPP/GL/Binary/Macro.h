//
// Created by scion on 11/3/2023.
//

#pragma once

#define NODISCARD [[nodiscard]]
#ifdef DEBUG
#define ALWAYS_INLINE inline
#else
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif // #if DEBUG

#ifdef DEBUG
#include <iostream>
#include <vector>
#endif // #ifdef DEBUG

#ifdef DEBUG
#define LOG(MSG) std::cout << MSG << std::endl;
#else
#define LOG(MSG) // NOTHING!
#endif

#define assertm(exp, msg) assert(((void) msg, exp))
#define GE_FAIL(ERR) assertm(false, ERR);
#define GE_ASSERT(COND, ERR) assertm(COND, ERR)

namespace gETF
{
	struct Serializable;
}

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)

#ifndef CONST_GLOBAL
#define GLOBAL inline const
#endif
#ifndef CONST_GLOBAL
#define CONST_GLOBAL inline constexpr const
#endif

#define GET_CONST(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() const { return FIELD; }
#define GET(TYPE, ACCESSOR, FIELD) \
	NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() { return FIELD; } \
	GET_CONST(const TYPE, ACCESSOR, FIELD)

#define SET(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE FIELD##_) { FIELD = FIELD##_; }
#define SET_XVAL(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE&& FIELD##_) { FIELD = std::move(FIELD##_); }

#define GET_SET(TYPE, ACCESSOR, FIELD) \
    GET(TYPE, ACCESSOR, FIELD)    \
    SET(TYPE, ACCESSOR, FIELD)

#define GET_SET_VALUE(TYPE, ACCESSOR, FIELD) \
	GET_VALUE(TYPE, ACCESSOR, FIELD)    \
	SET(TYPE, ACCESSOR, FIELD)

#define OPERATOR_EQUALS_T(TYPE, INTYPE) \
    TYPE& operator=(const INTYPE& o) \
    { \
        if((TYPE*) &o == this) return *this; \
        this->~TYPE(); \
        new(this) TYPE(o); \
        return *this; \
    }

#define OPERATOR_EQUALS_XVAL_T(TYPE, INTYPE) \
    TYPE& operator=(INTYPE&& o) noexcept \
    { \
        if((TYPE*) &o == this) return *this; \
        this->~TYPE(); \
        new(this) TYPE(std::move(o)); \
        return *this; \
    }

#define OPERATOR_EQUALS(TYPE) OPERATOR_EQUALS_T(TYPE, TYPE)
#define OPERATOR_EQUALS_XVAL(TYPE) OPERATOR_EQUALS_XVAL_T(TYPE, TYPE)

#define OPERATOR_EQUALS_BOTH(TYPE) \
    OPERATOR_EQUALS(TYPE); \
    OPERATOR_EQUALS_XVAL(TYPE);

#define OPERATOR_CAST_CONST(TYPE, FIELD) ALWAYS_INLINE operator TYPE() const { return FIELD; }
#define OPERATOR_CAST(TYPE, FIELD) \
	ALWAYS_INLINE operator TYPE() { return FIELD; } \
	OPERATOR_CAST_CONST(const TYPE, FIELD);

// Yapping about newline >: (