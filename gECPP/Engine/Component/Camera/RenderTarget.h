//
// Created by scion on 12/1/2023.
//

#pragma once

#include <Graphics/Buffer/FrameBuffer.h>
#include <Engine/Utility/RelativePointer.h>
#include <Engine/Renderer/GPU.h>

namespace gE
{
	class Camera;

	template<class T, GLenum TARGET>
	class Attachment
	{
	 public:
		template<typename... ARGS>
		explicit Attachment(API::FrameBuffer& f, ARGS&&... args) : _texture(&f.GetWindow(), std::forward<ARGS>(args)...)
		{
			if constexpr(TARGET == GL_DEPTH_ATTACHMENT || TARGET == GL_DEPTH_STENCIL_ATTACHMENT)
				f.SetDepthAttachment(_texture);
			else
				f.SetAttachment(TARGET - GL_COLOR_ATTACHMENT0, _texture);
		}

		Attachment(const Attachment&) = delete;
		Attachment(Attachment&&) = delete;

		ALWAYS_INLINE T* operator->() { return &_texture; }
		ALWAYS_INLINE const T* operator->() const { return &_texture; }

		ALWAYS_INLINE T& operator*() { return _texture; }
		ALWAYS_INLINE const T& operator*() const { return _texture; }

		ALWAYS_INLINE explicit operator T&() { return _texture; }
		ALWAYS_INLINE explicit operator const T&() const { return _texture; }
		ALWAYS_INLINE explicit operator T*() { return &_texture; }
		ALWAYS_INLINE explicit operator const T*() { return &_texture; }

		GET(T&, , _texture)

	 private:
		T _texture;
	};

 	class IRenderTarget
	{
	 public:
		virtual ~IRenderTarget() = default;

		explicit IRenderTarget(Entity& owner, Camera& camera);

		GET(Camera&, Camera, *_camera);
		GET(Entity&, Owner, *_owner);
		GET(API::FrameBuffer&, FrameBuffer, _frameBuffer);
 		GET(Window&, Window, *_window);

		virtual bool Setup(float, Camera*) { return true; }
		virtual void RenderDependencies(float) {};
		virtual void RenderPass(float, Camera*) = 0;
		virtual void PostProcessPass(float) {};
 		virtual void GetGPUCameraOverrides(GPU::Camera&) const {};

		inline void Bind() const { _frameBuffer.Bind(); }

	 private:
		RelativePointer<Camera> _camera;
		RelativePointer<Entity> _owner;
		API::FrameBuffer _frameBuffer;
 		Window* _window;
	};

	template<class T>
	class RenderTarget : public IRenderTarget
	{
	public:
		using CAMERA_T = T;
		using TEX_T = typename T::TEX_T;
		using IRenderTarget::IRenderTarget;

		GET(T&, Camera, (T&) IRenderTarget::GetCamera());
		GET_CONST(typename T::SIZE_TYPE, Size, GetCamera().GetSize());
	};

	class IDepthTarget
	{
	 public:
		explicit IDepthTarget(API::Texture& d) : _depth(d) {};

		GET(API::Texture&, Depth, *_depth);

	 private:
		RelativePointer<API::Texture> _depth;
	};

	class IColorTarget
	{
	 public:
		explicit IColorTarget(API::Texture& col) : _color(col) {};

		GET(API::Texture&, Color, *_color);
	 private:
		RelativePointer<API::Texture> _color;
	};
}
