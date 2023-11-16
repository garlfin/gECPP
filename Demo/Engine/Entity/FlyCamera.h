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
		gE::DefaultPipeline::AttachmentsDefault | gE::DefaultPipeline::AttachmentTAA | gE::DefaultPipeline::AttachmentScreenSpace
	};

	class FlyCamera : public gE::PerspectiveCamera
	{
	 public:
		explicit FlyCamera(gE::Window* window) :
			gE::PerspectiveCamera(window, gE::CameraSettings2D(FlyCameraSettings, window->GetSize()), nullptr, &window->GetCameras()),
			_movement(this)
		{
		}

	 private:
		Movement _movement;
	};
}