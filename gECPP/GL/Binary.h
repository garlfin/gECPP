//
// Created by scion on 8/16/2023.
//

#pragma once

#include "GLMath.h"


#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif
#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

namespace gETF { struct Serializable; }

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#ifndef MIN
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
#define BIT_SIZE(X) (sizeof(decltype(X)) * 8)

namespace gETF
{
	class SerializationBuffer
	{
	 public:
		explicit SerializationBuffer(u64 initialSize = 32)
			: _buf(new u8[initialSize] {}), _size(0), _allocLen(initialSize)
		{
		}

		template<class T>
		void PushPtr(T* t, u32 count = 1)
		{
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

		template<class T>
		ALWAYS_INLINE void Push(const T& t) { PushPtr<const T>(&t); }
		NODISCARD ALWAYS_INLINE u8* Data() const { return _buf; }
		NODISCARD ALWAYS_INLINE u64 Length() const { return _size; }

		void PushString(const char* ptr);
		void StrCat(const char* str);

		~SerializationBuffer()
		{
			delete[] _buf;
		}

	 private:
		u8* _buf;
		u64 _size, _allocLen;

		void Realloc(u64 newSize);
	};

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count);

	template<>
	inline void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count);
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

char* ReadString(u8*& ptr);

u8* ReadFile(const char* name, u32& length, bool binary = false);

inline u8* ReadFile(const char* name, bool binary)
{
	u32 len;
	return ReadFile(name, len, binary);
}
