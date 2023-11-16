//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	class DirectionalLight : public OrthographicCamera
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale);

		GET_CONST(const GL::Texture2D*, Depth, GetDepthAttachment());

		void OnRender(float delta) override;
	};

	class LightManager : public TypedManager<Camera>
	{
	 public:
		LightManager() = default;

		DirectionalLight* Sun;
	};
}
