//
// Created by scion on 11/3/2023.
//

#pragma once

#include <GL/Math.h>
#include "Macro.h"

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
		ALWAYS_INLINE void Push(const T& t) { PushPtr<const T>(&t, 1); }

		template<class T>
		void PushPtr(T* t, u32 count);
		template<size_t C, class T>
		void PushPtr(T* t);

		ALWAYS_INLINE u8* PushEnd(u64 length);
		void PushLengthString(const char* ptr);
		void StrCat(const char* str, bool = true, char = 0);
		void FromFile(const char* file, bool binary = false);
		char* Find(const char* str, char = 0);

		NODISCARD ALWAYS_INLINE u8* Data() const
		{ return _buf; }

		NODISCARD ALWAYS_INLINE u64 Length() const
		{ return _size; }

		~SerializationBuffer()
		{ free(_buf); }

	 private:
		u8* _buf;
		u64 _size, _alloc;

		inline void SafeMemCpy(const u8* ptr, u64 len, u64 offset);
		void Realloc(u64 newSize);
	};

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count);
}

template<class T>
void gETF::SerializationBuffer::PushPtr(T* t, u32 count)
{
	if(!count) return;
	if constexpr(std::is_base_of_v<Serializable, T>)
		for(u32 i = 0; i < count; i++) t[i].Deserialize(*this);
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
	if constexpr(std::is_base_of_v<Serializable, T>)
		for(u32 i = 0; i < C; i++) t[i].Deserialize(*this);
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
