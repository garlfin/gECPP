//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <gETF/File.h>
#include <Graphics/Shader/Shader.h>

namespace gE
{
	class CubemapCapture;

	class CubemapTarget : public RenderTarget<CameraCubemap>, public IDepthTarget
	{
	 public:
		explicit CubemapTarget(CameraCubemap&);

		GET(API::TextureCube&, Color, _color.Get());
		GET(API::TextureCube&, Depth, _depth.Get());
		GET(CubemapCapture&, Owner, (CubemapCapture&) IRenderTarget::GetOwner());

		void RenderDependencies(float d) override;
		void RenderPass(float, Camera*) override;

	 private:
		Attachment<API::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::TextureCube, GL_COLOR_ATTACHMENT0> _color;
	};

	class CubemapCapture final : public Entity, public Managed<CubemapCapture>
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET(API::TextureCube&, Color, _target.GetColor());
		GET(CameraCubemap&, Camera, _camera);

		void GetGPUCubemap(GPU::Cubemap&);

	 private:
		CameraCubemap _camera;
		CubemapTarget _target;
	};

	class CubemapManager final : public Manager<CubemapCapture>
	{
	 public:
		explicit CubemapManager(Window* window) : Manager<CubemapCapture>(), _window(window) {};

		Reference<API::TextureCube> Skybox{};

		void DrawSkybox();

		void OnUpdate(float delta) override {};
		void OnRender(float delta, Camera*) override;

	 private:
		Window* _window;

		SmartPointer<API::VAO> _skyboxVAO;
		SmartPointer<API::Shader> _skyboxShader;
	};
}
