//
// Created by scion on 9/2/2023.
//

#include "VAO.h"
#include "Mesh.h"

namespace GL
{
	VAO::VAO(gE::Window* window, const Mesh& settings) :
		Asset(window), _settings(settings),
		_bufferBuffer(new u8[sizeof(Buffer<u8>) * settings.BufferCount])
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < settings.BufferCount; i++)
		{
			const BufferSettings& bufSettings = settings.Buffers[i];
			Buffer<u8>& buf = _buffers[i];
			new(&buf) Buffer<u8>(window, bufSettings.Stride * bufSettings.Count, bufSettings.Data);

			glVertexArrayVertexBuffer(ID, i, buf.Get(), 0, bufSettings.Stride);
		}

		for(u8 i = 0; i < settings.FieldCount; i++)
		{
			const VAOField& field = settings.Fields[i];
			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.Buffer);
			glVertexArrayAttribFormat(ID, field.Index, field.TypeCount, field.Type, GL_FALSE, field.Offset);
		}
	}

	void VAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		SubMesh& mesh = _settings.Meshes[index];
		glDrawArraysInstanced(GL_TRIANGLES, mesh.Offset * 3, mesh.Count * 3, instanceCount);
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		SubMesh& mesh = _settings.Meshes[index];
		glDrawElementsInstanced(GL_TRIANGLES, mesh.Count * 3, _settings.Triangles.Type, (void*) u64(mesh.Offset * 3), instanceCount);
	}

	/*void VAO::Realloc(u32 vertexCount, void* data = nullptr)
	{
		_buffers->Realloc(GetFieldsSize(_fields) * sizeof(float) * vertexCount, data);
		_vertexCount = vertexCount;
		glVertexArrayVertexBuffer(ID, 0, _buffers.Get(), 0, sizeof(float) * GetFieldsSize(_fields));
	}*/

	VAO::~VAO()
	{
		glDeleteVertexArrays(1, &ID);
		for(u8 i = 0; i < _settings.BufferCount; i++) _buffers[i].~Buffer<u8>();
		operator delete[](_bufferBuffer);
	}

	VAO* VAO::Create(gE::Window* window, const Mesh& settings)
	{
		if(settings.TriangleMode == gETF::TriangleMode::Simple)
			return new IndexedVAO(window, settings);
		else return new VAO(window, settings);
	}

	Mesh::Mesh(const Mesh& o) :
		BufferCount(o.BufferCount),
		Buffers(new BufferSettings[BufferCount]),
		MeshCount(o.MeshCount),
		Meshes((SubMesh*) memcpy(new SubMesh[MeshCount], o.Meshes, MeshCount * sizeof(SubMesh))),
		FieldCount(o.FieldCount),
		Fields((VAOField*) memcpy(new VAOField[FieldCount], o.Fields, FieldCount * sizeof(VAOField)))
	{
		for(u8 i = 0; i < BufferCount; i++) Buffers[i] = o.Buffers[i];
	}

	Mesh::Mesh(const gETF::Mesh& mesh)
	{
		MeshCount = mesh.MaterialCount;
		Meshes = new SubMesh[MeshCount];
		for(u8 i = 0; i < MeshCount; i++) Meshes[i] = SubMesh(mesh.Materials[i]);

		BufferCount = 1 + (mesh.TriangleMode == gETF::TriangleMode::Simple); // TODO improve for future use
		Buffers = new BufferSettings[BufferCount];
		for(u8 i = 0; i < BufferCount; i++) Buffers[i].Index = i;

		FieldCount = mesh.FieldCount;
		Fields = new VAOField[FieldCount];

		for(u8 i = 0; i < FieldCount; i++) Fields[i] = VAOField(mesh.Fields[i], Buffers[0]);
		for(u8 i = 0; i < FieldCount; i++) Fields[i].CopyData(mesh.Fields[i], Buffers[0]);

		TriangleMode = mesh.TriangleMode;
		if(TriangleMode == gETF::TriangleMode::Simple)
		{
			Triangles = VAOField(mesh.Triangles, Buffers[1]);
			Triangles.CopyData(mesh.Triangles, Buffers[1]);
		}
	}

	SubMesh::SubMesh(const gETF::MaterialSlot& slot) : Offset(slot.Offset), Count(slot.Count)
	{
	}

	VAOField::VAOField(const gETF::VertexField& field, BufferSettings& buf) :
		Count(field.Count), Type(field.Type), TypeCount(field.TypeCount),
		Index(field.Index), Buffer(buf.Index), Offset(buf.Stride)
	{
#ifdef DEBUG
		std::cout << "FIELD | NAME: " << field.Name << ", TYPE: " << field.Type;
		std::cout << ", SIZE: " << GetSizeOfGLType(Type) * TypeCount << ", OFFSET: " << (u16) buf.Stride << '\n';
#endif
		buf.Stride += GetSizeOfGLType(Type) * TypeCount;
		buf.Count = MAX(buf.Count, field.Count);
	}

	void VAOField::CopyData(const gETF::VertexField& field, BufferSettings& buf) const
	{
		if(!buf.Data) buf.Data = new u8[buf.Stride * buf.Count];

		u8 dataStride = GetSizeOfGLType(Type) * TypeCount;
		u8* src = (u8*) field.Data, *dst = buf.Data + Offset;

		for(u32 i = 0; i < Count; i++)
		{
			memcpy(dst, src, dataStride);
			src += dataStride;
			dst += buf.Stride;
		}
	}

	IndexedVAO::IndexedVAO(gE::Window* window, const Mesh& settings)
		: VAO(window, settings)
	{
		glVertexArrayElementBuffer(ID, _buffers[settings.Triangles.Buffer].Get());
	}
}