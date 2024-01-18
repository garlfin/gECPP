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
		explicit SerializationBuffer(u64 initialSize = 32) : _buf((u8*) malloc(initialSize)), _size(0), _alloc(initialSize)
		{
		}

		void Push(const SerializationBuffer& t);

		template<class T> ALWAYS_INLINE void PushSerializable(const Serializable<T>& t, T s){ PushSerializablePtr<1, T>(&t, s); }
		template<class T> ALWAYS_INLINE void Push(T t) { PushPtr<1, T>(&t); }

		template<class T> void PushSerializablePtr(const Serializable<T>* t, T s, u32 count);
		template<size_t C, class T> void PushSerializablePtr(const Serializable<T>* t, T s);

		template<class T> void PushPtr(const T* t, u32 count);
		template<size_t C, class T> void PushPtr(const T* t);

		ALWAYS_INLINE u8* ReallocAdd(u64 length);

		void PushPrefixedString(const char* ptr);
		void StrCat(const char* str, bool = true, char = 0);
		char* Find(const char* str, char = 0);

		void FromFile(const char* file, bool binary = false);
		void ToFile(const char* file);

		NODISCARD ALWAYS_INLINE u8* Data() const { return _buf; }
		NODISCARD ALWAYS_INLINE u64 Length() const { return _size; }

		~SerializationBuffer() { free(_buf); }

	 private:
		u8* _buf;
		u64 _size, _alloc;

		void Realloc(u64 newSize);
	};

	template<class T>
	void SerializationBuffer::PushSerializablePtr(const Serializable<T>* t, T s, u32 count)
	{
		if(!count) return;
		for(u32 i = 0; i < count; i++) t[i].Deserialize(*this, s);
	}

	template<size_t C, class T>
	void SerializationBuffer::PushSerializablePtr(const Serializable<T>* t, T s)
	{
		static_assert(C > 0);
		for(u32 i = 0; i < C; i++) t[i].Deserialize(*this, s);
	}

	template<class T>
	void SerializationBuffer::PushPtr(const T* t, u32 count)
	{
		static_assert(std::is_trivially_copyable_v<T>, "T IS NOT COPYABLE!");

		if(!count) return;
		const size_t size = sizeof(T) * count;
		u64 oldSize = _size;
		Realloc(_size + size);
		memcpy(_buf + oldSize, t, size);
	}

	template<size_t C, class T>
	void SerializationBuffer::PushPtr(const T* t)
	{
		static_assert(std::is_trivially_copyable_v<T>, "T IS NOT COPYABLE!");
		static_assert(C > 0);

		constexpr size_t size = sizeof(T) * C;
		u64 oldSize = _size;
		Realloc(_size + size);
		memcpy(_buf + oldSize, t, size);
	}

	u8* SerializationBuffer::ReallocAdd(u64 length)
	{
		u64 preSize = _size;
		Realloc(_size + length);
		return _buf + preSize;
	}
}

