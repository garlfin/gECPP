//
// Created by scion on 9/2/2023.
//

#pragma once

#include <GL/Binary.h>
#include <gEModel/gETF/File.h>

namespace GL
{
	struct SubMesh
	{
		SubMesh(const gETF::MaterialSlot& slot);
		SubMesh() = default;

		u32 Offset = 0;
		u32 Count = 0;
	};

	struct BufferSettings
	{
		u32 Count = 0;
		u8 Stride = 0;
		u8 Index = 0;
		u8* Data = nullptr;

		~BufferSettings() { delete[] Data; }
	};

	struct VAOField
	{
		VAOField(const gETF::VertexField&, BufferSettings&);
		VAOField() = default;

		void CopyData(const gETF::VertexField&, BufferSettings&) const;

		u32 Count = 0;
		u32 Type = 0;
		u8 TypeCount = 0;
		u8 Index = 0;
		u8 Buffer = 0;
		u8 Offset = 0;
	};

	struct Mesh
	{
		Mesh(const Mesh& o);
		explicit Mesh(const gETF::Mesh& mesh);
		Mesh() = default;

		u8 BufferCount = 0;
		u8 MeshCount = 0;
		u8 FieldCount = 0;
		gETF::TriangleMode TriangleMode = gETF::TriangleMode::None;

		BufferSettings* Buffers = nullptr;
		SubMesh* Meshes = nullptr;
		VAOField* Fields = nullptr;
		VAOField Triangles {};

		~Mesh() { delete[] Buffers; delete[] Meshes; delete[] Fields; }
	};


}