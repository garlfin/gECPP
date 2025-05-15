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
			GPU::Buffer<std::byte>& bufSettings = _settings->Buffers[i];
			Buffer<std::byte>& buffer = _buffers[i];

			PlacementNew(buffer, window, move(bufSettings));
			bufSettings = move(buffer.GetSettings());

			glVertexArrayVertexBuffer(ID, i, buffer.Get(), 0, bufSettings.Stride);
		}

		for(u8 i = 0; i < _settings->Counts.FieldCount; i++)
		{
			const GPU::VertexField& field = _settings->Fields[i];

			glEnableVertexArrayAttrib(ID, field.Index);
			glVertexArrayAttribBinding(ID, field.Index, field.BufferIndex);
			glVertexArrayAttribFormat(ID, field.Index, field.ElementCount, (GLenum) field.ElementType, field.Normalized, field.Offset);
		}
	}

	void IVAO::UpdateBufferDirect(u8 i, std::span<std::byte> data, size_t offset) const
	{
		_buffers[i].UpdateDataDirect(data, offset);
	}

	void IVAO::UpdateBuffer(u8 i, GPU::Buffer<std::byte>&& buf) const
	{
		Buffer<std::byte>& dst = _buffers[i];

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
		glDrawArraysInstanced(ToGLEnum(PrimitiveType), material.Offset * (size_t) PrimitiveType, material.Count * (size_t) PrimitiveType, instanceCount);
	}

	API_SERIALIZABLE_IMPL(IndexedVAO), IVAO(window, *this)
	{
		PlacementNew(_indicesBuffer, window, move(IndicesBuffer));
		IndicesBuffer = move(_indicesBuffer.GetSettings());

		glVertexArrayElementBuffer(ID, _indicesBuffer.Get());
	}

	void IndexedVAO::Draw(u8 index, u16 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		const GPU::MaterialSlot& material = Materials[index];
		glDrawElementsInstanced(ToGLEnum(PrimitiveType), material.Count * (size_t) PrimitiveType, IndicesFormat, (void*) (sizeof(u32) * material.Offset * (size_t) PrimitiveType), instanceCount);
	}

	void VAO::Draw(u32 count, const GPU::IndirectDraw* calls) const
	{
		if(!count) return;

		gE::DrawCallManager& drawManager = GetWindow().GetRenderers().GetDrawCallManager();

		u32 baseInstance = 0;
		for(u32 i = 0; i < count; i++)
		{
			IndirectDraw& to = ((IndirectDraw*) drawManager.GetDraws().GetData().Data())[i];
			const GPU::IndirectDraw& call = calls[i];
			const GPU::MaterialSlot& material = Materials[call.SubmeshIndex];

			to.InstanceCount = call.InstanceCount * GetWindow().RenderState.InstanceMultiplier;
			to.Count = material.Count * (size_t) PrimitiveType;
			to.BaseInstance = baseInstance;
			to.First = material.Offset * (size_t) PrimitiveType;

			baseInstance += call.InstanceCount;
		}

		drawManager.GetDraws().UpdateData(sizeof(IndirectDrawIndexed) * count);

		Bind();
		glMultiDrawArraysIndirect(ToGLEnum(PrimitiveType), nullptr, count, 0);
	}

	void VAO::DrawDirect(u32 count, u32 offset, u32 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		glDrawArraysInstanced(ToGLEnum(PrimitiveType), offset * (size_t) PrimitiveType, count * (size_t) PrimitiveType, instanceCount);
	}

	void IndexedVAO::Draw(u32 count, const GPU::IndirectDraw* calls) const
	{
		if(!count) return;

		gE::DrawCallManager& drawManager = GetWindow().GetRenderers().GetDrawCallManager();

		u32 baseInstance = 0;
		for(u32 i = 0; i < count; i++)
		{
			IndirectDrawIndexed& to = ((IndirectDrawIndexed*) drawManager.GetDraws().GetData().Data())[i];
			const GPU::IndirectDraw& call = calls[i];
			const GPU::MaterialSlot& material = Materials[call.SubmeshIndex];

			to.InstanceCount = call.InstanceCount * GetWindow().RenderState.InstanceMultiplier;
			to.Count = material.Count * (size_t) PrimitiveType;
			to.BaseInstance = baseInstance;
			to.BaseVertex = 0;
			to.FirstIndex = material.Offset * (size_t) PrimitiveType;

			baseInstance += call.InstanceCount;
		}

		drawManager.GetDraws().UpdateData<IndirectDrawIndexed>(count);

		Bind();
		glMultiDrawElementsIndirect(ToGLEnum(PrimitiveType), IndicesFormat, nullptr, count, 0);
	}

	void IndexedVAO::DrawDirect(u32 count, u32 offset, u32 instanceCount) const
	{
		if(!instanceCount) return;
		Bind();
		glDrawElementsInstanced(ToGLEnum(PrimitiveType), count * (size_t) PrimitiveType, IndicesFormat, (void*) (GLSizeOf(IndicesFormat) * offset), instanceCount);
	}

	void IndexedVAO::UpdateIndicesDirect(std::span<std::byte> data, size_t offset) const
	{
		_indicesBuffer.UpdateDataDirect(data, offset);
	}

	void IndexedVAO::UpdateIndices(GPU::Buffer<std::byte>&& buf) const
	{
		_indicesBuffer = API::Buffer(&GetWindow(), std::move(buf));
		glVertexArrayElementBuffer(ID, _indicesBuffer.Get());
	}
}
