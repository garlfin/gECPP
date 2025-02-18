//
// Created by scion on 9/2/2023.
//

#include "VAO.h"

#include <Window.h>

namespace GL
{
	IVAO::IVAO(gE::Window* window, GPU::VAO& vao): GLObject(window), _settings(vao), _buffers(_settings->Counts.BufferCount)
	{
		glCreateVertexArrays(1, &ID);

		for(u8 i = 0; i < _settings->Counts.BufferCount; i++)
		{
			GPU::Buffer<u8>& bufSettings = _settings->Buffers[i];
			Buffer<u8>& buffer = _buffers[i];

			PlacementNew(buffer, window, move(bufSettings));
			bufSettings = move(buffer.GetSettings());

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

	void IVAO::UpdateBufferDirect(u8 i, const void* data, size_t count, size_t offset)
	{
		_buffers[i].ReplaceDataDirect(data, count, offset);
	}

	void IVAO::UpdateBuffer(u8 i, GPU::Buffer<u8>&& buf)
	{
		Buffer<u8>& dst = _buffers[i];

		dst = API::Buffer(&GetWindow(), std::move(buf));
		glVertexArrayVertexBuffer(ID, i, dst.Get(), 0, dst->GetStride());
	}

	IVAO::~IVAO()
	{
		glDeleteVertexArrays(1, &ID);
	}

	API_SERIALIZABLE_IMPL(VAO), IVAO(window, *this)
	{
	}

	void VAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GPU::MaterialSlot& material = Materials[index];
		glDrawArraysInstanced(GL_TRIANGLES, material.Offset * 3, material.Count * 3, instanceCount);
	}

	API_SERIALIZABLE_IMPL(IndexedVAO), IVAO(window, *this)
	{
		PlacementNew(_triangleBuffer, window, move(TriangleBuffer));
		TriangleBuffer = move(_triangleBuffer.GetSettings());

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

			to.InstanceCount = call.InstanceCount * GetWindow().RenderState.InstanceMultiplier;
			to.Count = material.Count * 3;
			to.BaseInstance = baseInstance;
			to.First = material.Offset * 3;

			baseInstance += call.InstanceCount;
		}

		drawManager.UpdateDrawCalls(sizeof(IndirectDrawIndexed) * count);

		Bind();
		glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, count, 0);
	}

	void VAO::DrawDirect(u32 count, u32 offset, u32 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		glDrawArraysInstanced(GL_TRIANGLES, offset * 3, count * 3, instanceCount);
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

			to.InstanceCount = call.InstanceCount * GetWindow().RenderState.InstanceMultiplier;
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

	void IndexedVAO::DrawDirect(u32 count, u32 offset, u32 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		glDrawElementsInstanced(GL_TRIANGLES, count * 3, TriangleFormat, (void*) (GLSizeOf(TriangleFormat) * offset), instanceCount);
	}

	void IndexedVAO::UpdateIndicesDirect(const void* data, size_t count, size_t offset)
	{
		_triangleBuffer.ReplaceDataDirect(data, count, offset);
	}

	void IndexedVAO::UpdateIndices(GPU::Buffer<u8>&& buf)
	{
		_triangleBuffer = API::Buffer(&GetWindow(), std::move(buf));
		glVertexArrayElementBuffer(ID, _triangleBuffer.Get());
	}
}
