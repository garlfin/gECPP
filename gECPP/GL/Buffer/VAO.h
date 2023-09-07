#pragma once

#include "GL/gl.h"
#include "Buffer.h"
#include "Mesh.h"

namespace GL
{
	class VAO : public Asset
	{
	 public:
		VAO(gE::Window* window, const Mesh& settings);

		ALWAYS_INLINE void Bind() const final;
		virtual void Draw(u8 index, u16 instanceCount = 1) const;

		NODISCARD ALWAYS_INLINE const Mesh& GetSettings() const { return _settings; }
		ALWAYS_INLINE void ReplaceData(u8 buf, u32 count, void* data) const { _buffers[buf].ReplaceData((u8*)data, _settings.Buffers[buf].Stride); }
		// void Realloc(u32 vertexCount, void* data = nullptr); // TODO

		static VAO* Create(gE::Window*, const Mesh&);

		~VAO() override;

	 public:
		union
		{
			Buffer<uint8_t>* _buffers;
			u8* _bufferBuffer; // INCEPTION!
			// TODO come up with a better name than this 😭😭
		};

		const Mesh _settings;
	};

	class IndexedVAO final : public VAO
	{
	 public:
		friend class VAO;
		inline void Draw(u8 index, u16 instanceCount = 1) const override;

	 protected:
		IndexedVAO(gE::Window* window, const Mesh& settings);
	};

	void VAO::Bind() const
	{
		glBindVertexArray(ID);
	}
}