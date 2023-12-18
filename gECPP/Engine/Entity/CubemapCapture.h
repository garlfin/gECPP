//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <gEModel/gETF.h>
#include <GL/Shader/Shader.h>

namespace gE
{
	class CubemapCapture;

	class CubemapTarget : public RenderTarget<CameraCubemap>, public IDepthTarget
	{
	 public:
		explicit CubemapTarget(CubemapCapture&, CameraCubemap&);

		GET(GL::TextureCube&, Color, _color.Get());
		GET(GL::TextureCube&, Depth, _depth.Get());
		GET(CubemapCapture&, Owner, (CubemapCapture&) IRenderTarget::GetOwner());
		void RenderPass(float, Camera*) override;

	 private:
		Attachment<GL::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<GL::TextureCube, GL_COLOR_ATTACHMENT0> _color;
	};

	class CubemapCapture final : public Entity
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET(GL::TextureCube&, Color, _target.GetColor());
		GET(CameraCubemap&, Camera, _camera);

		void GetGLCubemap(GL::CubemapData&);

	 private:
		CameraCubemap _camera;
		CubemapTarget _target;
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
