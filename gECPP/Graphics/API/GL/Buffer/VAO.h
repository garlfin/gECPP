#pragma once

#include "Buffer.h"
#include "../../../Buffer/VAOSettings.h"
#include "Engine/Array.h"

namespace GL
{
	class VAO : public GLObject
	{
	 public:
		VAO(gE::Window* window, const VAOSettings& settings);

		GET_CONST(const GL::VAOSettings&, Slots, _settings);

		ALWAYS_INLINE void Bind() const final { glBindVertexArray(ID); }

		virtual void Draw(u8 index, u16 instanceCount = 1) const;

		~VAO() override;

	 protected:
		Array<Buffer<void>*> _buffers;
		const VAOSettings _settings;
	};

	class IndexedVAO final : public VAO
	{
	 public:
		IndexedVAO(gE::Window* window, const IndexedVAOSettings& settings);

		void Draw(u8 index, u16 instanceCount = 1) const override;

	 private:
		VertexField _triangles;
	};
}