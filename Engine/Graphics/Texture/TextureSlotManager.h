//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Texture.h"

#define GE_MAX_TEXTURE_SLOT 32

namespace API
{
	class Texture;
}

namespace GPU
{
	class TextureSlotManager
	{
	public:
		TextureSlotManager() = default;

		NODISCARD u8 Increment(const API::Texture* t);

		ALWAYS_INLINE void Reset() { _index = 0; }

	private:
		const API::Texture* _textures[GE_MAX_TEXTURE_SLOT]{};
		u8 _index = 0;
	};
}