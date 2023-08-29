#pragma once

#include "GL/gl.h"
#include "Buffer.h"

namespace GL
{
	template<bool DYNAMIC>
	class VAO : public Asset
	{
	 public:
		VAO(gE::Window* window, VAOFields fields, uint32_t vertexCount, void* data);

		virtual inline void Draw(u16 instanceCount, u32 offset = 0, u32 maxTri = 0) const
		{
			if(!instanceCount) return;
			Bind();
			glDrawArraysInstanced(GL_TRIANGLES, offset, maxTri ?: _vertexCount, instanceCount);
		}

		inline void Realloc(u32 vertexCount, void* data = nullptr)
		{
			_data.Realloc(GetFieldsSize(_fields) * sizeof(float) * vertexCount, data);
			_vertexCount = vertexCount;
			glVertexArrayVertexBuffer(ID, 0, _data.Get(), 0, sizeof(float) * GetFieldsSize(_fields));
		};

		inline void ReplaceData(u32 vertexCount, void* data = nullptr)
		{
			_data.ReplaceData((u8*) data, GetFieldsSize(_fields) * sizeof(float) * vertexCount);
		}

		inline void Bind() const final { glBindVertexArray(ID); }
		~VAO() override { glDeleteVertexArrays(1, &ID); }

	 private:
		uint32_t _vertexCount;
		Buffer<uint8_t> _data;
	};

	class IndexedVAO final : public VAO
	{
	 private:
		uint32_t _triangleCount;
		Buffer<u32> _index;

	 public:
		IndexedVAO(gE::Window* window, VAOFields fields, uint32_t vertexCount, void* vertData, uint32_t triCount, uint16_t* triData);
		inline void Draw(uint16_t count, uint32_t offset = 0, uint32_t maxTri = 0) const override
		{
			if(!count) return;
			VAO<DYNAMIC>::Bind();
			glDrawElementsInstanced(GL_TRIANGLES, (maxTri ?: _triangleCount) * 3, GL_UNSIGNED_SHORT, (void*) (offset * sizeof(uint16_t)), count);
		}
	};
}