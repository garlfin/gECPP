//
// Created by scion on 11/3/2023.
//

#pragma once

#define MOVE(x) std::move(x)
#define COPY_MOVE(x) MOVE(std::remove_cv_t<decltype(x)>(x))

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
#define LOG(MSG) std::cout << MSG << std::endl
#else
#define LOG(MSG) // NOTHING!
#endif

#define assertm(exp, msg) assert(((void) msg, exp))
#define GE_FAIL(ERR) assertm(false, ERR);
#define GE_ASSERT(COND, ERR) assertm(COND, ERR)

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)

#ifndef CONSTEXPR_GLOBAL
#define GLOBAL inline const
#endif
#ifndef CONSTEXPR_GLOBAL
#define CONSTEXPR_GLOBAL inline constexpr const
#endif

#define GET_CONST(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() const { return FIELD; }
#define GET(TYPE, ACCESSOR, FIELD) \
	NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() { return FIELD; } \
	GET_CONST(const TYPE, ACCESSOR, FIELD)

#define SET(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE ACCESSOR##_) { FIELD = ACCESSOR##_; }
#define SET_XVAL(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE&& FIELD##_) { FIELD = std::move(FIELD##_); }

#define GET_SET(TYPE, ACCESSOR, FIELD) \
    GET(TYPE, ACCESSOR, FIELD)    \
    SET(TYPE, ACCESSOR, FIELD)

#define GET_SET_VALUE(TYPE, ACCESSOR, FIELD) \
	GET_CONST(TYPE, ACCESSOR, FIELD)    \
	SET(TYPE, ACCESSOR, FIELD)

#define OPERATOR_EQUALS_XVAL_PROTO(TYPE) \
    TYPE(TYPE&&) noexcept; \
	TYPE& operator=(TYPE&&) noexcept;

#define OPERATOR_EQUALS_PROTO(TYPE) \
	TYPE(const TYPE&); \
	TYPE& operator=(const TYPE& ACCESSOR);

#define OPERATOR_EQUALS_XVAL_NAMESPACE(NAMESPACE, TYPE, ACCESSOR, CODE) \
    NAMESPACE##TYPE(TYPE&& ACCESSOR) noexcept { CODE; } \
	NAMESPACE##TYPE& NAMESPACE##operator=(TYPE&& ACCESSOR) noexcept \
	{ \
		if(&ACCESSOR == this) return *this; \
		CODE; \
		return* this; \
	}

#define OPERATOR_EQUALS_NAMESPACE(NAMESPACE, TYPE, ACCESSOR, CODE) \
	NAMESPACE##TYPE(const TYPE& ACCESSOR) { CODE; } \
	NAMESPACE##TYPE& NAMESPACE##operator=(const TYPE& ACCESSOR) \
	{ \
		if(&ACCESSOR == this) return *this; \
		CODE; \
		return *this; \
	}

#define OPERATOR_EQUALS_XVAL(TYPE, ACCESSOR, CODE) OPERATOR_EQUALS_XVAL_NAMESPACE(, TYPE, ACCESSOR, CODE)
#define OPERATOR_EQUALS(TYPE, ACCESSOR, CODE) OPERATOR_EQUALS_NAMESPACE(, TYPE, ACCESSOR, CODE)

#define OPERATOR_CAST_CONST(TYPE, FIELD) ALWAYS_INLINE operator TYPE() const { return FIELD; }
#define OPERATOR_CAST(TYPE, FIELD) \
	ALWAYS_INLINE operator TYPE() { return FIELD; } \
	OPERATOR_CAST_CONST(const TYPE, FIELD);

#define DELETE_COPY_CONSTRUCTOR(TYPE) \
	TYPE(TYPE&&) noexcept = delete; \
	TYPE(const TYPE&) = delete; \
	TYPE& operator=(TYPE&&) noexcept = delete; \
	TYPE& operator=(const TYPE&) = delete;

// Yapping about newline >: (