#pragma once

#include "GL/GL.h"
#include "Buffer.h"

namespace gETF
{
	struct Mesh;
	struct MaterialSlot;
}

namespace GL
{
	class VAO : public Asset
	{
	 public:
		VAO(gE::Window* window, const gETF::Mesh* settings);

		GET_CONST(const gETF::Mesh*, Settings, _settings);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }

		virtual void Draw(u8 index, u16 instanceCount = 1) const;

		~VAO() override;

	 protected:
		union
		{
			Buffer<void>* _buffers;
			void* _bufferBuffer; // Allocation for _buffers, so I don't have to construct them on the spot.
			// TODO come up with a better name than this 😭😭
		};

		const gETF::Material* _settings;
	};

	class IndexedVAO final : public VAO
	{
	 public:
		IndexedVAO(gE::Window* window, const gETF::Mesh* settings);

		void Draw(u8 index, u16 instanceCount = 1) const override;

		friend class VAO;
	};
}