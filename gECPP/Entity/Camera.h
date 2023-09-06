//
// Created by scion on 9/5/2023.
//

#pragma once

#include "Entity.h"
#include <GL/Texture/Texture.h>

namespace gE
{
	class Camera;

	typedef void(*RenderPass)(Camera*);
	typedef void(*PostProcessPass)(Camera*, GL::Texture* in, GL::Texture* out);

	class Camera : public Entity
	{
	 public:


		NODISCARD ALWAYS_INLINE GL::TextureSize GetSize() const { return _texture->GetSize(); }
	 private:
		GL::Texture* _texture;
		RenderPass _renderPass;
		PostProcessPass _postProcessPasses[];
	};
}
