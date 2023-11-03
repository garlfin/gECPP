//
// Created by scion on 10/25/2023.
//

#pragma once

#include "GL/Binary/Binary.h"

#define GL_MAX_TEXTURE_SLOT 32

namespace GL
{
	class Texture;

	class TextureSlotManager
	{
	 public:
		TextureSlotManager() = default;

		NODISCARD u8 Increment(const Texture* t);
		ALWAYS_INLINE void Reset() { _index = 0; }

	 private:
		const Texture* _textures[GL_MAX_TEXTURE_SLOT] {};
	 	u8 _index = 0;
	};
}