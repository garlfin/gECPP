//
// Created by scion on 11/3/2023.
//
#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

using std::move;
using namespace std::string_literals;
using namespace std::string_view_literals;

#if defined(__GNUC__) || defined(__clang__)
	#define GE_COMPILER_GCC
#elif defined(_MSC_VER)
	#define GE_COMPILER_MSVC
#endif

#ifdef GE_COMPILER_GCC
#include <cxxabi.h>
	#define PRETTY_FUNCTION __PRETTY_FUNCTION__
	#define TRAP __builtin_trap
	#define DEBUGBREAK __debugbreak
	#define FORCE_IMPL __attribute__((used))
	#define NO_IMPL __attribute__((unused))
#endif

#ifdef GE_COMPILER_MSVC
	#define PRETTY_FUNCTION __FUNCSIG__
	#define TRAP __debugbreak
#endif

#define COPY(x) std::remove_cvref_t<decltype(x)>(x)
#define COPY_MOVE(x) std::move(COPY(x))

template<class T>
using add_const_before_pointer = std::conditional_t<std::is_pointer_v<T>, const std::remove_pointer_t<T>*, const T>;

template<class T>
using add_const_before_ref = std::conditional_t<std::is_reference_v<T>, const std::remove_reference_t<T>&, const T>;

template<class T, bool CONST>
using add_const_conditional = std::conditional_t<CONST, add_const_before_pointer<T>, T>;

#ifdef GE_COMPILER_GCC
inline std::string demangle(const char* name)
{
	int status = -1;
	const std::unique_ptr<char, void(*)(void*)> res
	{
		abi::__cxa_demangle(name, nullptr, nullptr, &status),
		std::free
	};

	return !status ? res.get() : name;
}
#else
inline std::string demangle(const char* name)
{
	return std::string(name);
}
#endif

template<class T, typename... ARGS>
T& PlacementNew(T& to, ARGS&&... args)
{
	to.~T();
	new(&to) T(std::forward<ARGS>(args)...);
	return to;
}

#define NODISCARD [[nodiscard]]
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

#ifdef DEBUG
	#define ALWAYS_INLINE inline
#else
#ifdef GE_COMPILER_MSVC
	#define ALWAYS_INLINE __forceinline
#endif
#ifdef GE_COMPILER_GCC
	#define ALWAYS_INLINE inline __attribute__((always_inline))
#endif
#endif // #if DEBUG

#define NOINLINE __declspec(noinline)

#ifdef DEBUG
#include <vector>
#endif // #ifdef DEBUG

#define assertm(exp, msg) assert(((void) msg, exp))

#ifdef DEBUG
	#define GE_ASSERTM(COND, ERR) { bool cond = COND; if(!cond) __debugbreak(); assertm(cond, ERR); }
	#define GE_ASSERT(COND) { bool cond = COND; if(!cond) __debugbreak(); assert(cond); }
#else
	#define GE_ASSERTM(COND, ERR) assertm(cond, ERR)
	#define GE_ASSERT(COND) assert(cond)
#endif

#define UNPACK(...) __VA_ARGS__

#define GE_FAIL(ERR) GE_ASSERTM(false, ERR)

#define BIT_FIELD(FIELD, INDEX) ((FIELD) >> (INDEX) & 1)
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)
#define BIT_FIELD_ALIGN u8 : 0

#ifndef MIN
	#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef MAX
	#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif

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
    GET_CONST(add_const_before_ref<TYPE>, ACCESSOR, FIELD) \
    SET(add_const_before_ref<TYPE>, ACCESSOR, FIELD)

#define OPERATOR_MOVE_PROTO(TYPE) \
    TYPE(TYPE&&); \
	TYPE& operator=(TYPE&&);

#define OPERATOR_COPY_PROTO(TYPE) \
	TYPE(const TYPE&); \
	TYPE& operator=(const TYPE& ACCESSOR);

#define OPERATOR_MOVE_IMPL(TYPE, DESTRUCTOR, INHERIT, CODE) \
    TYPE(TYPE&& o) noexcept \
	{ \
		if(&o == this) return; \
		[&]() { INHERIT; CODE; }(); \
	} \
	TYPE& operator=(TYPE&& o) noexcept \
	{ \
		if(&o == this) return *this; \
		DESTRUCTOR; \
		[&]() { INHERIT; CODE; }(); \
		return* this; \
	}

#define OPERATOR_COPY_IMPL(TYPE, DESTRUCTOR, INHERIT, CODE) \
	TYPE(const TYPE& o) \
	{ \
		[&]() { INHERIT; CODE; }(); \
	} \
	TYPE& operator=(const TYPE& o) \
	{ \
		if(&o == this) return *this; \
		DESTRUCTOR; \
		[&]() { INHERIT; CODE; }(); \
		return *this; \
	}

#define OPERATOR_MOVE(TYPE, DESTRUCTOR, SUPER, CODE) \
	OPERATOR_MOVE_IMPL(TYPE, TYPE::DESTRUCTOR(), SUPER::operator=(move(o)), CODE)

#define OPERATOR_COPY(TYPE, DESTRUCTOR, SUPER, CODE) \
	OPERATOR_COPY_IMPL(TYPE, TYPE::DESTRUCTOR(), SUPER::operator=(o), CODE)

#define OPERATOR_MOVE_NOSUPER(TYPE, DESTRUCTOR, CODE) \
	OPERATOR_MOVE_IMPL(TYPE, TYPE::DESTRUCTOR(),, CODE)

#define OPERATOR_COPY_NOSUPER(TYPE, DESTRUCTOR, CODE) \
	OPERATOR_COPY_IMPL(TYPE, TYPE::DESTRUCTOR(),, CODE)

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
	public: \
	TYPE(const TYPE&) = default; \
	TYPE& operator=(const TYPE&) = default

#define DEFAULT_OPERATOR_MOVE(TYPE) \
	public: \
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

#define ENUM_OPERATOR_UNARY(TYPE, OP) \
	inline constexpr TYPE operator OP(TYPE a) \
	{ \
		return (TYPE) ~std::underlying_type_t<TYPE>(a); \
	} \

#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#define offsetbetween(TYPE, FIELDFROM, FIELDTO) (offsetof(TYPE, FIELDTO) - offsetof(TYPE, FIELDFROM))
#define sizebetween(TYPE, FIELDFROM, FIELDTO) (offsetof(TYPE, FIELDTO) + sizeof(TYPE::FIELDTO) - offsetof(TYPE, FIELDFROM))
#define offsetof_memptr(TYPE, PTR) ((size_t) &((TYPE*) nullptr->*PTR))
#define sizeof_array(ARR) (sizeof(ARR) / sizeof(*(ARR))