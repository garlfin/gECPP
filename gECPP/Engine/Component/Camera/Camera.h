//
// Created by scion on 9/5/2023.
//

#pragma once

#include <GL/Texture/Texture.h>
#include <GL/Buffer/FrameBuffer.h>

#include "Engine/Entity/Entity.h"
#include "Engine/Array.h"
#include "Engine/Manager.h"
#include "Settings.h"
#include "Timing.h"

namespace GL
{
	struct Camera;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

namespace gE
{
	class Camera : public Component
	{
	 public:
		Camera(Entity*, Manager*, GL::TextureSize2D, IRenderTarget&, const ICameraSettings&);

		inline void OnUpdate(float delta) override { }
		inline void OnRender(float delta) override { Draw(delta, nullptr); }

		virtual void GetGLCamera(GL::Camera&);
		void Draw(float, Camera*);

		GET(IRenderTarget&, Target, _target);

		GET_CONST(CameraTiming, Timing, _settings.Timing);
		GET_CONST(gE::ClipPlanes, ClipPlanes, _settings.ClipPlanes);
		GET_CONST(const ICameraSettings&, Settings, _settings);
		GET_CONST(const glm::mat4&, Projection, Projection);
		GET_CONST(GL::TextureSize2D, ViewportSize, _viewportSize);

	 protected:
		virtual void UpdateProjection() = 0;

		glm::mat4 Projection;
		u32 Frame = 0;

	 private:
		ICameraSettings _settings;
		IRenderTarget& _target;

		GL::TextureSize2D _viewportSize;
		bool _isProjectionInvalid = true;
	};

	class Camera2D : public Camera
	{
	 public:
		typedef RenderTarget<Camera2D> TARGET_TYPE;
		typedef GL::TextureSize2D SIZE_TYPE;

		Camera2D(Entity*, Manager*, TARGET_TYPE&, const CameraSettings2D&);

		GET(TARGET_TYPE&, Target, (TARGET_TYPE&) Camera::GetTarget());
		GET_CONST(SIZE_TYPE, Size, GetViewportSize());
		GET_CONST(float, Aspect, (float) GetSize().x / GetSize().y);

		void GetGLCamera(GL::Camera& camera) override;
	};

	class PerspectiveCamera : public Camera2D
	{
	 public:
		PerspectiveCamera(Entity*, Manager*, TARGET_TYPE&, const PerspectiveCameraSettings&);

		template<AngleType T = AngleType::Degree>
		NODISCARD ALWAYS_INLINE float GetFOV() const
		{
			if constexpr(T == AngleType::Radian)
				return _fov;
			return glm::degrees(_fov);
		}

		template<AngleType T = AngleType::Degree>
		ALWAYS_INLINE void SetFOV(float fov)
		{
			if constexpr(T == AngleType::Radian)
				_fov = fov;
			else
				_fov = glm::radians(fov);
		}

	 protected:
		void UpdateProjection() override;

	 private:
		float _fov;
	};

	class OrthographicCamera : public Camera2D
	{
	 public:
		OrthographicCamera(Entity*, Manager*, TARGET_TYPE&, const OrthographicCameraSettings&);

		GET_CONST(const glm::vec4&, Scale, _orthographicScale);

	 protected:
		void UpdateProjection() override;

	 private:
		glm::vec4 _orthographicScale;
	};

	class Camera3D : public Camera
	{
	 public:
		typedef RenderTarget<Camera3D> TARGET_TYPE;
		typedef GL::TextureSize3D SIZE_TYPE;

		Camera3D(Entity*, Manager*, TARGET_TYPE&, const CameraSettings3D&);

		GET(TARGET_TYPE&, Target, (TARGET_TYPE&) Camera::GetTarget());
		GET_CONST(SIZE_TYPE, Size, SIZE_TYPE(GetViewportSize(), _sizeZ));

		void GetGLCamera(GL::Camera&) override;

	 protected:
		void UpdateProjection() override;

	 private:
		const GL::TextureSize1D _sizeZ;
	};

	class CameraCubemap : public Camera
	{
	 public:
		typedef RenderTarget<CameraCubemap> TARGET_TYPE;
		typedef GL::TextureSize1D SIZE_TYPE;

		CameraCubemap(Entity*, Manager*, TARGET_TYPE&, const CameraSettings1D&);

		GET(TARGET_TYPE&, Target, (TARGET_TYPE&) Camera::GetTarget());
		GET_CONST(SIZE_TYPE, Size, GetViewportSize().x);

		void GetGLCamera(GL::Camera& camera) override;

	 protected:
		void UpdateProjection() override;
	};

	class CameraManager : public ComponentManager<Camera>
	{
	 public:
		using ComponentManager<Camera>::ComponentManager;

		Camera* CurrentCamera = nullptr;
		GL::Texture2D* Color = nullptr;
	};
}

#pragma clang diagnostic pop