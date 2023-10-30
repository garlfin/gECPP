//
// Created by scion on 8/16/2023.
//

#pragma once

#include "Math.h"
#include <cstring>
#include <algorithm>

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

#define assertm(exp, msg) assert(((void) msg, exp))
#define GE_FAIL(ERR) assertm(false, ERR);
#define GE_ASSERT(COND, ERR) assertm(COND, ERR);

namespace gETF { struct Serializable; }

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)
#ifdef GLOBAL
#error GLOBAL ALREADY DEFINED
#else
#define GLOBAL inline constexpr const
#endif

#define GET_CONST_VALUE(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() const { return FIELD; }
#define GET_CONST(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE const TYPE Get##ACCESSOR() const { return FIELD; }
#define GET(TYPE, ACCESSOR, FIELD) NODISCARD ALWAYS_INLINE TYPE Get##ACCESSOR() { return FIELD; } \
								   GET_CONST(TYPE, ACCESSOR, FIELD)

#define SET(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE FIELD##_) { FIELD = FIELD##_; }
#define SET_XVAL(TYPE, ACCESSOR, FIELD) ALWAYS_INLINE void Set##ACCESSOR(TYPE&& FIELD##_) { FIELD = std::move(FIELD##_); }

#define GET_SET(TYPE, ACCESSOR, FIELD) \
	GET(TYPE, ACCESSOR, FIELD)	\
	SET(TYPE, ACCESSOR, FIELD)

#define GET_SET_VALUE(TYPE, ACCESSOR, FIELD) \
	GET_CONST_VALUE(TYPE, ACCESSOR, FIELD) \
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

size_t strlenc(const char*, char);
size_t strlencLast(const char*, char, char = 0);
char* strdupc(const char*, char);

const char* IncrementLine(const char* s, char d = '\n');

namespace gETF
{
	class SerializationBuffer
	{
	 public:
		explicit SerializationBuffer(u64 initialSize = 32)
			: _buf((u8*) malloc(initialSize)), _size(0), _alloc(initialSize)
		{
		}

		template<class T>
		void PushPtr(T* t, u32 count);
		template<size_t C, class T>
		void PushPtr(T* t);
		template<class T>
		ALWAYS_INLINE void Push(const T& t) { PushPtr<const T>(&t, 1); }


		NODISCARD ALWAYS_INLINE u8* Data() const { return _buf; }
		NODISCARD ALWAYS_INLINE u64 Length() const { return _size; }

		ALWAYS_INLINE u8* PushEnd(u64 length);
		void PushLengthString(const char* ptr);
		void StrCat(const char* str, bool = true, char = 0);
		void FromFile(const char* file, bool binary = false);
		char* Find(const char* str, char = 0);

		~SerializationBuffer() { free(_buf); }

	 private:
		u8* _buf;
		u64 _size, _alloc;

		inline void SafeMemCpy(const u8* ptr, u64 len, u64 offset);
		void Realloc(u64 newSize);
	};

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count);
}

template<typename T>
T Read(u8*& src)
{
	static_assert(!std::is_base_of_v<gETF::Serializable, T>, "DONT USE THIS W/ SERIALIZABLES!");
	T t = *(T*) src;
	src += sizeof(T);
	return t;
}

template<typename T>
void Read(u8*& src, T* ts, u32 count)
{
	if constexpr(std::is_base_of_v<gETF::Serializable, T>)
	{
		for (u32 i = 0; i < count; i++)
			ts[i].Serialize(src);
	} else
	{
		memcpy(ts, src, count * sizeof(T));
		src += count * sizeof(T);
	}
}

template<typename T, u32 COUNT>
void Read(u8*& src, T* ts)
{
	memcpy(ts, src, COUNT * sizeof(T));
	src += COUNT * sizeof(T);
}

template<u32 LENGTH>
bool StrCmp(const char* a, const char* b)
{
	for(u32 i = 0; i < LENGTH; i++)
		if(a[i] != b[i]) return false;
	return true;
}

template<u32 LENGTH>
bool StrCmp(const char* a, const char(&b)[LENGTH])
{
	for(u32 i = 0; i < LENGTH - 1; i++) // exclude null terminator
		if(a[i] != b[i]) return false;
	return true;
}

char* ReadLengthString(u8*& ptr);
u8* ReadFile(const char* name, u32& length, bool binary = false);

inline u8* ReadFile(const char* name, bool binary = false)
{
	u32 len;
	return ReadFile(name, len, binary);
}

template<class T>
void gETF::SerializationBuffer::PushPtr(T* t, u32 count)
{
	if(!count) return;
	if constexpr (std::is_base_of_v<Serializable, T>)
		for (u32 i = 0; i < count; i++) t[i].Deserialize(*this);
	else
	{
		static_assert(std::is_trivially_copyable_v<T>, "T IS NOT COPYABLE!");
		const size_t size = sizeof(T) * count;
		u64 oldSize = _size;
		Realloc(_size + size);
		memcpy(_buf + oldSize, t, size);
	}
}

void gETF::SerializationBuffer::SafeMemCpy(const u8* ptr, u64 len, u64 offset)
{
	assertm(offset + len < _alloc, "WROTE OUT OF BOUNDS!");
	memcpy(_buf + offset, ptr, std::min(offset + len, _alloc));
}

template<size_t C, class T>
void gETF::SerializationBuffer::PushPtr(T* t)
{
	if constexpr (std::is_base_of_v<Serializable, T>)
		for (u32 i = 0; i < C; i++) t[i].Deserialize(*this);
	else
	{
		static_assert(std::is_trivially_copyable_v<T>, "T IS NOT COPYABLE!");
		static constexpr size_t size = sizeof(T) * C;
		u64 oldSize = _size;
		Realloc(_size + size);
		memcpy(_buf + oldSize, t, size);
	}
}

u8* gETF::SerializationBuffer::PushEnd(u64 length)
{
	u64 preSize = _size;
	Realloc(_size + length);
	return _buf + preSize;
}

template<class T>
void RemoveFirstFromVec(std::vector<T>& vec, const T& t)
{
	auto f = std::find(vec.begin(), vec.end(), t);
	if(f == vec.end()) return;
	std::iter_swap(f, vec.end() - 1);
	vec.pop_back();
}