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
		explicit CubemapTarget(CameraCubemap&);

		GET(GL::TextureCube&, Color, _color.Get());
		GET(GL::TextureCube&, Depth, _depth.Get());
		GET(CubemapCapture&, Owner, (CubemapCapture&) IRenderTarget::GetOwner());

		void RenderDependencies(float d) override;
		void RenderPass(float, Camera*) override;

	 private:
		Attachment<GL::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<GL::TextureCube, GL_COLOR_ATTACHMENT0> _color;
	};

	class CubemapCapture final : public Entity, public Managed<CubemapCapture>
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET(GL::TextureCube&, Color, _target.GetColor());
		GET(CameraCubemap&, Camera, _camera);

		void GetGLCubemap(GL::Cubemap&);

	 private:
		CameraCubemap _camera;
		CubemapTarget _target;
	};

	class CubemapManager final : public Manager<CubemapCapture>
	{
	 public:
		explicit CubemapManager(Window* window) : Manager<CubemapCapture>(), _window(window) {};

		Reference<GL::TextureCube> Skybox{};

		void DrawSkybox();

		void OnUpdate(float delta) override {};
		void OnRender(float delta, Camera*) override;

	 private:
		Window* _window;

		SmartPointer<GL::VAO> _skyboxVAO;
		SmartPointer<GL::Shader> _skyboxShader;
	};
}
