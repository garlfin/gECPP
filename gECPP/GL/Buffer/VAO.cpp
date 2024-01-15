//
// Created by scion on 9/2/2023.
//

#include "VAO.h"

namespace GL
{
	VAO::VAO(gE::Window* window, const VAOSettings& settings) : Asset(window),
		_settings(settings),
		_buffers(settings.BufferCount)
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < settings.BufferCount; i++)
		{
			const GL::BufferSettings& bufferSettings = settings.Buffers[i];
			const size_t size = bufferSettings.Stride * bufferSettings.Count;
			const auto* buf = _buffers[i] = new Buffer<void>(window, size, bufferSettings.Data);

			glVertexArrayVertexBuffer(ID, i, buf->Get(), 0, bufferSettings.Stride);
		}

		for(u8 i = 0; i < settings.FieldCount; i++)
		{
			const GL::VertexField& field = settings.Fields[i];

			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.BufferIndex);
			glVertexArrayAttribFormat(ID, field.Index, field.ElementCount, field.ElementType, field.Normalized, field.Offset);
		}
	}

	void VAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GL::MaterialSlot& mesh = _settings.Materials[index];
		glDrawArraysInstanced(GL_TRIANGLES, mesh.Offset * 3, mesh.Count * 3, instanceCount);
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GL::MaterialSlot& mesh = _settings.Materials[index];
		glDrawElementsInstanced(GL_TRIANGLES, mesh.Count * 3, _triangles.ElementType, (void*) u64(mesh.Offset * 3), instanceCount);
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
		for(u8 i = 0; i < _settings.FieldCount; i++) delete _buffers[i];
	}

	IndexedVAO::IndexedVAO(gE::Window* window, const GL::IndexedVAOSettings& settings)
		: VAO(window, settings), _triangles(settings.Triangles)
	{
		glVertexArrayElementBuffer(ID, _buffers[settings.Triangles.BufferIndex]->Get());
	}
}