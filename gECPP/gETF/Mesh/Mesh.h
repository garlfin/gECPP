//
// Created by scion on 1/23/2024.
//

#pragma once

#include <Prototype.h>
#include <gETF/Serializable.h>
#include <GL/Buffer/VAOSettings.h>
#include <Engine/AssetManager.h>
#include <GL/Buffer/VAO.h>

namespace gETF
{
	struct VertexBuffer : public Serializable<File>, public GL::BufferSettings
	{
		SERIALIZABLE_PROTO;

		inline void Free() { free(Data); Data = nullptr; }
		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data; }
		NODISCARD ALWAYS_INLINE u64 Size() const { return Count * Stride; }

		~VertexBuffer() override { Free(); }
	};

	struct VertexField : public Serializable<File>, public GL::VertexField
	{
		SERIALIZABLE_PROTO;

		char Name[4];
	};

	struct MaterialSlot : public Serializable<File>, public GL::MaterialSlot
	{
		SERIALIZABLE_PROTO;
	};

	enum class TriangleMode : u8
	{
		None,
		Simple,
		Complex
	};

	struct Mesh : public Serializable<File>
	{
		SERIALIZABLE_PROTO;

		std::string Name;

		TriangleMode TriangleMode = TriangleMode::None;

		VertexField Triangles;
		Array<VertexBuffer> Buffers;
		Array<VertexField> Fields;
		Array<MaterialSlot> Materials;

		gE::SmartPointer<GL::VAO> VAO;

		void Free() { for(u8 i = 0; i < Buffers.Count(); i++) Buffers[i].Free(); }
		void CreateVAO(gE::Window*);

		void GetVAOSettings(GL::VAOSettings&) const;
		void GetVAOSettings(GL::IndexedVAOSettings&) const;
	};
}