#pragma once

#include <GL/Math.h>
#include "Prototype.h"
#include <Engine/AssetManager.h>
#include <GL/Buffer/VAO.h>

#define GETF_VERSION 1

namespace gETF
{
	struct File;
	struct Mesh;
	struct VertexBuffer;
	struct MaterialSlot;
	struct Serializable;
	struct SerializationBuffer;

	// TODO
	// struct Scene;
	// struct Node;

	struct VertexBuffer : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 Index = 0;
		u8 Stride = 0;
		u32 Count = 0;

		void* Data = nullptr;

		inline void Free() { free(Data); Data = nullptr; }
		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data; }

		~VertexBuffer() { free(Data); }
	};

	struct VertexField : public Serializable
	{
		SERIALIZABLE_PROTO;

		const char* Name;

		GLenum ElementType = 0;

		u8 Index = 0;
		u8 BufferIndex = 0;
		u8 ElementCount = 0;
		u8 Offset = 0;

		~VertexField() { delete[] Name; }
	};

	struct MaterialSlot : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 MaterialIndex = 0;
		u32 Offset = 0;
		u32 Count = 0;
	};

	enum class TriangleMode : u8
	{
		None,
		Simple,
		Complex
	};

	struct Mesh : public Serializable
	{
		SERIALIZABLE_PROTO;

		const char* Name = nullptr;

		u8 MaterialCount = 0;
		u8 BufferCount = 0;
		u8 FieldCount = 0;
		TriangleMode TriangleMode = TriangleMode::None;

		VertexField Triangles;
		VertexBuffer* Buffers = nullptr;
		VertexField* Fields = nullptr;
		MaterialSlot* Materials = nullptr;

		gE::SmartPointer<GL::VAO> VAO { nullptr };

		void Free() const { for(u8 i = 0; i < BufferCount; i++) Buffers[i].Free(); }
		void CreateVAO(gE::Window*);

		~Mesh();
	};

	struct File : public Serializable
	{
		SERIALIZABLE_PROTO;

		u8 ReferenceCount = 0;
		gE::Reference<File>* References = nullptr;

		u8 MaterialCount = 0;

		u8 MeshCount = 0;
		gE::Reference<Mesh>* Meshes = nullptr;

		~File() { delete[] Meshes; }
	};

	File& Read(const char*, File&);
	NODISCARD File* Read(const char*);

	typedef gE::Reference<Mesh> MeshReference;
}
