//
// Created by scion on 9/2/2023.
//

#include "VAO.h"

#include "Engine/Window.h"

namespace GL
{
	IVAO::IVAO(gE::Window* window, GPU::VAO& vao): GLObject(window), _settings(vao), _buffers(_settings->Counts.BufferCount)
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < _settings->Counts.BufferCount; i++)
		{
			GPU::Buffer<u8>& bufSettings = _settings->Buffers[i];
			Buffer<u8>& buffer = _buffers[i];

			SAFE_CONSTRUCT(buffer, Buffer, window, move(bufSettings));
			buffer.Free();
			glVertexArrayVertexBuffer(ID, i, buffer.Get(), 0, bufSettings.Stride);
		}

		for(u8 i = 0; i < _settings->Counts.FieldCount; i++)
		{
			const GPU::VertexField& field = _settings->Fields[i];

			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.BufferIndex);
			glVertexArrayAttribFormat(ID, field.Index, field.ElementCount, field.ElementType, field.Normalized, field.Offset);
		}
	}

	VAO::VAO(gE::Window* window, SUPER&& INTERNAL_SETTINGS) : SUPER(move(INTERNAL_SETTINGS)), IVAO(window, *this)
	{
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

	IndexedVAO::IndexedVAO(gE::Window* window, SUPER&& INTERNAL_SETTINGS) : SUPER(move(INTERNAL_SETTINGS)), IVAO(window, *this)
	{
		SAFE_CONSTRUCT(_triangleBuffer, Buffer, window, move(TriangleBuffer));
		_triangleBuffer.Free();
		glVertexArrayElementBuffer(ID, _triangleBuffer.Get());
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GPU::MaterialSlot& material = Materials[index];
		glDrawElementsInstanced(GL_TRIANGLES, material.Count * 3, TriangleFormat, (void*) (sizeof(u32) * material.Offset * 3), instanceCount);
	}

	void VAO::Draw(u32 count, const GPU::IndirectDraw* calls) const
	{
		if(!count) return;

		gE::DrawCallManager& drawManager = GetWindow().GetRenderers().GetDrawCallManager();

		u32 baseInstance = 0;
		for(u32 i = 0; i < count; i++)
		{
			IndirectDraw& to = ((IndirectDraw*) drawManager.IndirectDraws)[i];
			const GPU::IndirectDraw& call = calls[i];
			const GPU::MaterialSlot& material = Materials[call.Material];

			to.InstanceCount = call.InstanceCount;
			to.Count = material.Count * 3;
			to.BaseInstance = baseInstance;
			to.First = material.Offset * 3;

			baseInstance += call.InstanceCount;
		}

		drawManager.UpdateDrawCalls(sizeof(IndirectDrawIndexed) * count);

		Bind();
		glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, count, 0);
	}

	void IndexedVAO::Draw(u32 count, const GPU::IndirectDraw* calls) const
	{
		if(!count) return;

		gE::DrawCallManager& drawManager = GetWindow().GetRenderers().GetDrawCallManager();

		u32 baseInstance = 0;
		for(u32 i = 0; i < count; i++)
		{
			IndirectDrawIndexed& to = ((IndirectDrawIndexed*) drawManager.IndirectDraws)[i];
			const GPU::IndirectDraw& call = calls[i];
			const GPU::MaterialSlot& material = Materials[call.Material];

			to.InstanceCount = call.InstanceCount * GetWindow().State.InstanceMultiplier;
			to.Count = material.Count * 3;
			to.BaseInstance = baseInstance;
			to.BaseVertex = 0;
			to.FirstIndex = material.Offset * 3;

			baseInstance += call.InstanceCount;
		}

		drawManager.UpdateDrawCalls(sizeof(IndirectDrawIndexed) * count);

		Bind();
		glMultiDrawElementsIndirect(GL_TRIANGLES, TriangleFormat, nullptr, count, 0);
	}
}
