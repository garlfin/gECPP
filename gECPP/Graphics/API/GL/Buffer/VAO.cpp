//
// Created by scion on 9/2/2023.
//

#include "VAO.h"

namespace GL
{
	VAO::VAO(gE::Window* window, SUPER&& settings) : SUPER(MOVE(settings)), IVAO(window, *this)
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < settings.Counts.BufferCount; i++)
		{
			const GPU::Buffer<u8>& bufSettings = settings.Buffers[i];
			Buffer<u8>& buffer = Buffers[i];

			Buffers[i] = Buffers(window, MOVE(bufSettings));
			glVertexArrayVertexBuffer(ID, i, buffer.Get(), 0, bufSettings.Stride);
		}

		for(u8 i = 0; i < settings.Counts.FieldCount; i++)
		{
			const GPU::VertexField& field = settings.Fields[i];

			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.BufferIndex);
			glVertexArrayAttribFormat(ID, field.Index, field.ElementCount, field.ElementType, field.Normalized, field.Offset);
		}
	}

	void VAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GPU::MaterialSlot& material = Materials[index];
		glDrawArraysInstanced(GL_TRIANGLES, material.Offset * 3, material.Count * 3, instanceCount);
	}

	VAO::~VAO()
	{
		glDeleteVertexArrays(1, &ID);
	}

	IndexedVAO::IndexedVAO(gE::Window* window, SUPER&& settings) : SUPER(MOVE(settings)), IVAO(window, *this)
	{
		_triangleBuffer = Buffer<u8>(window, MOVE(settings.TriangleBuffer));
		glVertexArrayElementBuffer(ID, _triangleBuffer.Get());
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GPU::MaterialSlot& material = Materials[index];
		glDrawElementsInstanced(GL_TRIANGLES, material.Count * 3, TriangleFormat, (void*) (sizeof(u32) * mesh.Offset * 3), instanceCount);
	}

}
