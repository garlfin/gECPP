#pragma once

#include "GL/gl.h"
#include "Buffer.h"
#include "VAOSettings.h"

namespace GL
{
	class VAO : public Asset
	{
	 public:
		ALWAYS_INLINE void Bind() const final;
		virtual void Draw(u8 index, u16 instanceCount = 1) const;

		ALWAYS_INLINE void ReplaceData(u8 buf, u32 count, void* data);
		// void Realloc(u32 vertexCount, void* data = nullptr); // TODO

		static VAO* Create(gE::Window*, const VAOSettings*);

		~VAO() override;

	 protected:
		VAO(gE::Window* window, const VAOSettings* settings);

		union
		{
			Buffer<uint8_t>* _buffers;
			u8* _bufferBuffer; // INCEPTION!
			// TODO come up with a better name than this 😭😭
		};

		const VAOSettings* _settings;
	};

	class IndexedVAO final : public VAO
	{
	 public:
		friend class VAO;
		inline void Draw(u8 index, u16 instanceCount = 1) const override;

	 protected:
		IndexedVAO(gE::Window* window, const VAOSettings* settings);
	};

	void VAO::ReplaceData(u8 buf, u32 count, void* data)
	{
		_buffers[buf].ReplaceData((u8*)data, _settings->Buffers[buf].Stride);
	}

	void VAO::Bind() const
	{
		glBindVertexArray(ID);
	}
}