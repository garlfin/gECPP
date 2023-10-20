//
// Created by scion on 9/2/2023.
//

#include "VAO.h"
#include <gEModel/gETF/File.h>

namespace GL
{
	VAO::VAO(gE::Window* window, const gETF::Mesh* settings) : Asset(window),
		_settings(settings), _bufferBuffer(malloc(sizeof(Buffer<void>) * settings->BufferCount))
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < settings->BufferCount; i++)
		{
			Buffer<void>& buf = _buffers[i];
			const gETF::VertexBuffer& bufSettings = settings->Buffers[i];
			const size_t size = bufSettings.Stride * bufSettings.Count;

			new(&buf) Buffer<void>(window, size, bufSettings.Data);

			glVertexArrayVertexBuffer(ID, bufSettings.Index, buf.Get(), 0, bufSettings.Stride);
		}

		for(u8 i = 0; i < settings->FieldCount; i++)
		{
			const gETF::VertexField& field = settings->Fields[i];

			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.BufferIndex);
			glVertexArrayAttribFormat(ID, field.Index, field.ElementCount, field.ElementType, false, field.Offset);
		}
	}

	void VAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const gETF::MaterialSlot& mesh = _settings->Materials[index];
		glDrawArraysInstanced(GL_TRIANGLES, mesh.Offset * 3, mesh.Count * 3, instanceCount);
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const gETF::MaterialSlot& mesh = GetSettings()->Materials[index];
		glDrawElementsInstanced(GL_TRIANGLES, mesh.Count * 3, _settings->Triangles.ElementType, (void*) u64(mesh.Offset * 3), instanceCount);
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
		for(u8 i = 0; i < _settings->FieldCount; i++) _buffers[i].~Buffer<void>();
		free(_bufferBuffer);
	}

	IndexedVAO::IndexedVAO(gE::Window* window, const gETF::Mesh* settings)
		: VAO(window, settings)
	{
		glVertexArrayElementBuffer(ID, _buffers[settings->Triangles.BufferIndex].Get());
	}
}