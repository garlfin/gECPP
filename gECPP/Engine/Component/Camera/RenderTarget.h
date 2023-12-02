//
// Created by scion on 12/1/2023.
//

#pragma once

#include <GL/Buffer/FrameBuffer.h>
#include "Engine/Manager.h"

namespace gE
{
	class Camera;

 	class IRenderTarget : public GL::Asset
	{
	 public:
		explicit IRenderTarget(Camera&);

		GET(Camera&, Camera, _camera);
		GET(GL::FrameBuffer&, FrameBuffer, _frameBuffer);

		virtual void RenderPass() = 0;

		inline void Bind() const final { _frameBuffer.Bind(); }

	 private:
		Camera& _camera;
		GL::FrameBuffer _frameBuffer;
	};

	template<class T>
	class RenderTarget : public IRenderTarget
	{
	public:
		using IRenderTarget::IRenderTarget;

		GET(T&, Camera, (T&) IRenderTarget::GetCamera());
		GET_CONST(const T::SIZE_TYPE&, Size, GetCamera().GetSize());

		typedef T CAMERA_TYPE;
	};
}