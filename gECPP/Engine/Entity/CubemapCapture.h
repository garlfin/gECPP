//
// Created by scion on 10/23/2023.
//

#pragma once
#include "Entity.h"
#include <Engine/Component/Camera.h>
#include <gEModel/gETF.h>
#include <GL/Shader/Shader.h>

namespace gE
{
	class CubemapCapture final : public Entity
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET_CONST(GL::TextureCube*, Texture, _camera.GetAttachment(0));
		GET(CameraCubemap&, Camera, _camera);

		void GetGLCubemap(GL::CubemapData&);

	 private:
		CameraCubemap _camera;
	};

	class CubemapManager final : public TypedManager<CubemapCapture>
	{
	 public:
		explicit CubemapManager(Window* window) : TypedManager<CubemapCapture>(), _window(window) {};

		gE::Reference<GL::TextureCube> Skybox{};

		void DrawSkybox();
		void OnRender(float delta) override;

	 private:
		Window* _window;

		SmartPointer<GL::VAO> _skyboxVAO;
		SmartPointer<GL::Shader> _skyboxShader;
	};
}
