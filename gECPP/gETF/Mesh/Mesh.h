//
// Created by scion on 1/23/2024.
//

#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>
#include <Graphics/API/GL/Buffer/VAOSettings.h>
#include <Engine/AssetManager.h>
#include <Graphics/Buffer/VAO.h>

#define GETF_MESH_MAGIC "GMSH"
#define GETF_MESH_VERSION 1

namespace gETF
{
	struct VertexBuffer : public Serializable<Mesh&>
	{
		SERIALIZABLE_PROTO_T(VertexBuffer, Serializable<Mesh&>);

	 public:
		VertexBuffer() = default;

		u8 Stride = 0;
		u32 Count = 0;
		Array<u8> Data;

		inline void Free() { Data.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data.IsFree(); }
		NODISCARD ALWAYS_INLINE u64 Size() const { return Count * Stride; }

		NODISCARD explicit operator API::VertexBuffer() const
		{
			return API::VertexBuffer(Stride, Count, Data.Data());
		}

		~VertexBuffer() override { Free(); }
	};

	struct VertexField : public Serializable<Mesh&>
	{
		SERIALIZABLE_PROTO_T(VertexField, Serializable<Mesh&>);

	 public:
		VertexField() = default;

		GLenum ElementType;
		bool Normalized : 1;
		u8 BufferIndex : 7;
		u8 Index;
		u8 ElementCount;
		u8 Offset;
		char Name[4];

		NODISCARD explicit operator API::VertexField() const
		{
			return *(API::VertexField*) &ElementType;
		}
	};

	struct MaterialSlot : public Serializable<Mesh&>
	{
		SERIALIZABLE_PROTO_T(MaterialSlot, Serializable<Mesh&>);

	 public:
		MaterialSlot() = default;

		u32 Offset = 0;
		u32 Count = 0;

		NODISCARD ALWAYS_INLINE explicit operator API::MaterialSlot() const
		{
			// This will cause chaos one day
			return *(API::MaterialSlot*) &Offset;
		}
	};

	enum class TriangleMode : u8
	{
		None,
		Simple,
		Complex
	};

	struct Mesh : public Serializable<gE::Window*>
	{
		SERIALIZABLE_PROTO_T(Mesh, Serializable<gE::Window*>);

	 public:
		Mesh() = default;

		gE::Window* Window;
		u8 Version = GETF_MESH_VERSION;

		TriangleMode TriMode = TriangleMode::None;

		VertexField Triangles;
		Array<VertexBuffer> Buffers;
		Array<VertexField> Fields;
		Array<MaterialSlot> Materials;

		gE::SmartPointer<API::VAO> VAO;

		void Free() { for(u8 i = 0; i < Buffers.Count(); i++) Buffers[i].Free(); }
		void CreateVAO(gE::Window*);

	 private:
		void GetVAOSettings(API::VAOSettings&) const;
		void GetVAOSettings(API::IndexedVAOSettings&) const;
	};
}