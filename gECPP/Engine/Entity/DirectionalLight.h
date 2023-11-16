//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	class DirectionalLight : public Entity
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale);

		GET_CONST(const GL::Texture2D*, Depth, _camera.GetDepthAttachment());

		void OnRender(float delta) override;
	 private:
		OrthographicCamera _camera;
	};
}
