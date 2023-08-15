//
// Created by scion on 8/10/2023.
//

#pragma once

#include <GLMath.h>

#define GETF_VERSION 1
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define TRIANGLE_MODE_SIMPLE 0
#define TRIANGLE_MODE_COMPLEX 1

#define NODISCARD [[nodiscard]]
#define ALWAYS_INLINE __attribute__((always_inline))
#define SERIALIZABLE_PROTO  void Deserialize(SerializationBuffer& buf) const override;\
							void Serialize(void*& ptr) const override;

namespace gETF
{
	struct Header;
	struct Mesh;
	struct VertexField;
	struct MaterialSlot;
	struct Serializable;
	struct SerializationBuffer;

	// TODO
	// struct Scene;
	// struct Node;

	Header Import(const char*);

	struct Serializable
	{
		virtual void Deserialize(SerializationBuffer& buf) const = 0;
		virtual void Serialize(void*& ptr) const = 0;
	};

	struct VertexField : public Serializable
	{
		SERIALIZABLE_PROTO;

		const char* Name;

		u8 Index;

		u32 Type;
		u8 TypeCount;

		u32 Count;

		void* Data;

		~VertexField()
		{
			delete[] (u8*)Data;
			delete[] Name;
		}
	};

	struct MaterialSlot : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialIndex;
		u32 Offset;
		u32 Count;
	};

	struct Mesh : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialCount;
		u8 FieldCount;
		u8 TriangleMode;

		VertexField* Fields;
		MaterialSlot* Materials;

		~Mesh()
		{
			delete[] Fields;
			delete[] Materials;
		}
	};

	struct Header : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MeshCount;
		Mesh* Meshes;

		~Header()
		{
			delete[] Meshes;
		}
	};

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
				for (u32 i = 0; i < count; i++)
					t[i].Deserialize(*this);
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

		void Realloc(u64 newSize)
		{
			u8* oldBuf = _buf;

			_buf = new u8[newSize] {};
			memcpy(_buf, oldBuf, MIN(newSize, _allocLen));
			_allocLen = newSize;
			delete[] oldBuf;
		}
	};

	template<>
	void SerializationBuffer::PushPtr<const SerializationBuffer>(const SerializationBuffer* t, u32 count);
}







