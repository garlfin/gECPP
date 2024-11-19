//
// Created by scion on 11/3/2023.
//
#pragma once

#include <utility>

using std::move;

#if defined(__GNUC__)
	#define GE_COMPILER_GCC
#elif defined(_MSC_VER)
	#define GE_COMPILER_MSVC
#endif

#ifdef GE_COMPILER_GCC
	#define PRETTY_FUNCTION __PRETTY_FUNCTION__
	#define TRAP __builtin_trap
#endif

#ifdef GE_COMPILER_MSVC
	#define PRETTY_FUNCTION __FUNCSIG__
	#define TRAP __debugbreak
#endif

#define COPY_MOVE(x) std::move(std::remove_cvref_t<decltype(x)>(x))

#define SAFE_CONSTRUCT_NAMESPACE(TO, TYPE, NAMESPACE, ...) \
	{ \
		(TO).~TYPE(); \
		new(&(TO)) NAMESPACE::TYPE(__VA_ARGS__); \
	}

#define SAFE_CONSTRUCT(TO, TYPE, ...) \
	{ \
		(TO).~TYPE(); \
		new(&(TO)) TYPE(__VA_ARGS__); \
	}

#define NODISCARD [[nodiscard]]
#ifdef DEBUG
#define ALWAYS_INLINE inline
#else
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#endif // #if DEBUG

#define NOINLINE __attribute__((noinline))

#ifdef DEBUG
#include <csignal>
#include <iostream>
#include <vector>
#endif // #ifdef DEBUG

#ifdef DEBUG
#define LOG(MSG) std::cout << MSG << '\n'
#define ERR(MSG) { TRAP(); std::cerr << MSG << std::endl; }
#else
#define LOG(MSG)
#define ERR(MSG) std::cerr << MSG << std::endl
#endif

#define assertm(exp, msg) assert(((void) msg, exp))

#ifdef DEBUG
	#define GE_ASSERT(COND, ERR) { bool cond = COND; if(!cond) TRAP(); assertm(cond, ERR); }
#else
	#define GE_ASSERT(COND, ERR) assertm(cond, ERR)
#endif

#define UNPACK(...) __VA_ARGS__

#define GE_FAIL(ERR) GE_ASSERT(false, ERR);

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)

#ifndef GLOBAL
#define GLOBAL inline const
#endif

#ifndef CONSTEXPR_GLOBAL
#define CONSTEXPR_GLOBAL inline constexpr
#endif

#define DEFAULT {}

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

#define OPERATOR_MOVE_PROTO(TYPE) \
    TYPE(TYPE&&); \
	TYPE& operator=(TYPE&&);

#define OPERATOR_COPY_PROTO(TYPE) \
	TYPE(const TYPE&); \
	TYPE& operator=(const TYPE& ACCESSOR);

// terrible non nested if statement emulator...
#define OPERATOR_MOVE_UNSAFE(TYPE, INHERIT, CODE) \
    TYPE(TYPE&& o) noexcept \
	{ \
		if(&o == this) return; \
		for(bool INTERNAL_BREAK = false; !INTERNAL_BREAK; INTERNAL_BREAK = true) { INHERIT; CODE; } \
	} \
	TYPE& operator=(TYPE&& o) noexcept \
	{ \
		if(&o == this) return *this; \
		this->~TYPE(); \
		for(bool INTERNAL_BREAK = false; !INTERNAL_BREAK; INTERNAL_BREAK = true) { INHERIT; CODE; } \
		return* this; \
	}

#define OPERATOR_COPY_UNSAFE(TYPE, INHERIT, CODE) \
	TYPE(const TYPE& o) \
	{ \
		for(bool INTERNAL_BREAK = false; !INTERNAL_BREAK; INTERNAL_BREAK = true) { INHERIT; CODE; } \
	} \
	TYPE& operator=(const TYPE& o) \
	{ \
		if(&o == this) return *this; \
		this->~TYPE(); \
		for(bool INTERNAL_BREAK = false; !INTERNAL_BREAK; INTERNAL_BREAK = true) { INHERIT; CODE; } \
		return *this; \
	}

#define OPERATOR_MOVE(TYPE, SUPER, CODE) \
	OPERATOR_MOVE_UNSAFE(TYPE, SUPER::operator=(move(o)), CODE)

#define OPERATOR_COPY(TYPE, SUPER, CODE) \
	OPERATOR_COPY_UNSAFE(TYPE, SUPER::operator=(o), CODE)

#define OPERATOR_MOVE_NOSUPER(TYPE, CODE) \
	OPERATOR_MOVE_UNSAFE(TYPE,, CODE)

#define OPERATOR_COPY_NOSUPER(TYPE, CODE) \
	OPERATOR_COPY_UNSAFE(TYPE,, CODE)

#define OPERATOR_CAST_CONST(TYPE, FIELD) ALWAYS_INLINE operator TYPE() const { return FIELD; }
#define OPERATOR_CAST(TYPE, FIELD) \
	ALWAYS_INLINE operator TYPE() { return FIELD; } \
	OPERATOR_CAST_CONST(const TYPE, FIELD);

#define DELETE_OPERATOR_COPY(TYPE) \
	TYPE(const TYPE&) = delete; \
	TYPE& operator=(const TYPE&) = delete

#define DELETE_OPERATOR_MOVE(TYPE) \
	TYPE(TYPE&&) = delete; \
	TYPE& operator=(TYPE&&) = delete

#define DELETE_OPERATOR_CM(TYPE) \
	DELETE_OPERATOR_COPY(TYPE); \
	DELETE_OPERATOR_MOVE(TYPE)

#define DEFAULT_OPERATOR_COPY(TYPE) \
	TYPE(const TYPE&) = default; \
	TYPE& operator=(const TYPE&) = default

#define DEFAULT_OPERATOR_MOVE(TYPE) \
	TYPE(TYPE&&) = default; \
	TYPE& operator=(TYPE&&) = default

#define DEFAULT_OPERATOR_CM(TYPE) \
	DEFAULT_OPERATOR_COPY(TYPE); \
	DEFAULT_OPERATOR_MOVE(TYPE)

#define ENUM_OPERATOR(TYPE, OP) \
	inline constexpr TYPE operator OP(TYPE a, TYPE b) \
	{ \
		return (TYPE) (std::underlying_type_t<TYPE>(a) OP std::underlying_type_t<TYPE>(b)); \
	} \
	inline TYPE& operator OP##=(TYPE& a, TYPE b) \
	{ \
		return a = (TYPE) (std::underlying_type_t<TYPE>(a) OP std::underlying_type_t<TYPE>(b)); \
	}

#define ENUM_OPERATOR_LOGICAL(TYPE, OP) \
	inline constexpr TYPE operator OP(TYPE a) \
	{ \
		return (TYPE) ~std::underlying_type_t<TYPE>(a); \
	} \

// Yapping about newline >: (