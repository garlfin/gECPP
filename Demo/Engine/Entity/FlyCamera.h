//
// Created by scion on 9/13/2023.
//

#pragma once

#include "GL/Buffer/VAO.h"
#include "Engine/Component/Camera.h"
#include "Engine/Window.h"
#include "Demo/Engine/Component/Movement.h"

namespace VoxelDemo
{
	gE::SizelessCameraSettings FlyCameraSettings
	{
		(gE::RenderPass) gE::DefaultPipeline::RenderPass2D,
		gE::ClipPlanes(0.1, 100),
		gE::DefaultCameraTiming,
		gE::DefaultPipeline::AttachmentsDefault | gE::DefaultPipeline::AttachmentTAA
	};

	class FlyCamera : public gE::Entity
	{
	 public:
		explicit FlyCamera(gE::Window* window) : gE::Entity(window),
			_camera(this, &window->GetCameras(), gE::CameraSettings2D(FlyCameraSettings, GetWindow().GetSize())),
			_movement(this)
		{
		}

		GET(gE::PerspectiveCamera&, Camera, _camera);

	 private:
		gE::PerspectiveCamera _camera;
		Movement _movement;
	};
}