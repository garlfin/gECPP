//
// Created by scion on 10/23/2023.
//

#pragma once

#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Asset/File.h>
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Shader/Shader.h>

#include "Entity.h"

namespace gE
{
	class CubemapCapture;

	class CubemapTarget : public RenderTarget<CameraCube>, public IDepthTarget
	{
	 public:
		explicit CubemapTarget(CameraCube&);

		GET(API::TextureCube&, Color, _color.Get());
		GET(API::TextureCube&, Depth, _depth.Get());
		GET(CubemapCapture&, Owner, (CubemapCapture&) IRenderTarget::GetOwner());

		void RenderDependencies(float d) override;
		void RenderPass(float, Camera*) override;

	 private:
		Attachment<API::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
		Attachment<API::TextureCube, GL_COLOR_ATTACHMENT0> _color;
	};

	class CubemapCapture final : public Entity
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET(API::TextureCube&, Color, _target.GetColor());
		GET(CameraCube&, Camera, _camera);

		void GetGPUCubemap(GPU::Cubemap&);

	 private:
		Managed<CubemapCapture> _cubemapManaged;
		CameraCube _camera;
		CubemapTarget _target;
	};

	class CubemapManager final : public Manager<Managed<CubemapCapture>>
	{
	 public:
		explicit CubemapManager(Window* window);

		Reference<API::TextureCube> Skybox = DEFAULT;

		void DrawSkybox() const;

		void OnRender(float delta, Camera*);

		void UseNearestCubemaps(const glm::vec3& point) const;

		~CubemapManager() override = default;

	 private:
		Window* _window = nullptr;

		API::VAO _skyboxVAO = DEFAULT;
		API::Shader _skyboxShader = DEFAULT;
	};
}
