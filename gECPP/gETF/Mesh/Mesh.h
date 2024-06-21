//
// Created by scion on 1/23/2024.
//

#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>
#include <GL/Buffer/VAOSettings.h>
#include <Engine/AssetManager.h>
#include <GL/Buffer/VAO.h>

#define GETF_MESH_MAGIC "GMSH"
#define GETF_MESH_VERSION 1

namespace gETF
{
	struct VertexBuffer : public Serializable<Mesh>, public GL::BufferSettings
	{
		SERIALIZABLE_PROTO_T(VertexBuffer, Serializable<Mesh>);

	 public:
		VertexBuffer() = default;

		inline void Free() { free(Data); Data = nullptr; }
		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data; }
		NODISCARD ALWAYS_INLINE u64 Size() const { return Count * Stride; }

		~VertexBuffer() override { Free(); }
	};

	struct VertexField : public Serializable<Mesh>, public GL::VertexField
	{
		SERIALIZABLE_PROTO_T(VertexField, Serializable<Mesh>);

	 public:
		VertexField() = default;

		char Name[4];
	};

	struct MaterialSlot : public Serializable<Mesh>, public GL::MaterialSlot
	{
		SERIALIZABLE_PROTO_T(MaterialSlot, Serializable<Mesh>);

	 public:
		MaterialSlot() = default;
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

		gE::SmartPointer<GL::VAO> VAO;

		void Free() { for(u8 i = 0; i < Buffers.Count(); i++) Buffers[i].Free(); }
		void CreateVAO(gE::Window*);

	 private:
		void GetVAOSettings(GL::VAOSettings&) const;
		void GetVAOSettings(GL::IndexedVAOSettings&) const;
	};
}