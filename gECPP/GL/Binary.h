//
// Created by scion on 8/16/2023.
//

#pragma once

#include "GLMath.h"

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

#define BIT_FIELD(FIELD, INDEX) (((FIELD) >> (INDEX)) & 1)
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

namespace gETF
{
	class SerializationBuffer
	{
	 public:
		explicit SerializationBuffer(u64 initialSize = 32)
			: _buf(new u8[initialSize] {}), _len(0), _allocLen(initialSize)
		{
		}
		~SerializationBuffer()
		{
			delete[] _buf;
		}

		template<class T>
		void PushPtr(T* t, u32 count = 1)
		{
			if constexpr(std::is_base_of_v<Serializable, T>)
			{
				for (u32 i = 0; i < count; i++) t[i].Deserialize(*this);
			}
			else
			{
				const size_t size = sizeof(T) * count;
				if (_len + size > _allocLen) Realloc(_allocLen + MAX(MIN(_allocLen, 1024), size));
				memcpy(_buf + _len, t, size);
				_len += size;
			}
		}

		template<class T>
		ALWAYS_INLINE void Push(const T& t)
		{
			PushPtr<const T>(&t);
		}

		NODISCARD ALWAYS_INLINE u8* Data() const
		{
			return _buf;
		}
		NODISCARD ALWAYS_INLINE u64 Length() const
		{
			return _len;
		}

		void PushString(const char* ptr)
		{
			u8 len = MIN(strlen(ptr), UINT8_MAX);
			Push(len);
			PushPtr(ptr, len);
		}

	 private:
		u8* _buf;
		u64 _len, _allocLen;

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

inline char* ReadString(u8*& ptr)
{
	u8 len = Read<u8>(ptr);
	char* str = new char[len + 1];
	Read<char>(ptr, str, len);
	str[len] = 0;
	return str;
}

inline u8* ReadFile(const char* name)
{
	FILE* file = fopen(name, "rb");

	fseek(file, 0, SEEK_END);
	u64 fileLen = ftell(file);
	u8* bin = new u8[fileLen];
	fseek(file, 0, SEEK_SET);

	fread(bin, fileLen, 1, file);

	return bin;
}
