//
// Created by scion on 12/1/2023.
//

#pragma once

#include <Graphics/Buffer/FrameBuffer.h>
#include <Engine/Component/Camera/Camera.h>
#include "Engine/Manager.h"

namespace gE
{
	template<class T, GLenum TARGET>
	class Attachment
	{
	 public:
		template<typename... ARGS>
		Attachment(API::FrameBuffer& f, ARGS&&... args) : _texture(&f.GetWindow(), std::forward<ARGS>(args)...)
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

 	class IRenderTarget : public API::APIObject
	{
	 public:
		explicit IRenderTarget(Entity&, Camera&);

		GET(Camera&, Camera, _camera);
		GET(Entity&, Owner, _owner);
		GET(API::FrameBuffer&, FrameBuffer, _frameBuffer);

		virtual bool Setup(float, Camera*) { return true; }
		virtual void RenderDependencies(float) {};
		virtual void RenderPass(float, Camera*) = 0;
		virtual void PostProcessPass(float) {};

		inline void Bind() const override { _frameBuffer.Bind(); }

	 private:
		Camera& _camera;
		Entity& _owner;
		API::FrameBuffer _frameBuffer;
	};

	template<class T>
	class RenderTarget : public IRenderTarget
	{
	public:
		using IRenderTarget::IRenderTarget;

		GET(T&, Camera, (T&) IRenderTarget::GetCamera());
		GET_CONST(T::SIZE_TYPE, Size, GetCamera().GetSize());

		typedef T CAMERA_TYPE;
	};

	class IDepthTarget
	{
	 public:
		explicit IDepthTarget(API::Texture& d) : _depth(d) {};

		GET(API::Texture&, Depth, _depth);

	 private:
		API::Texture& _depth;
	};

	class IColorTarget
	{
	 public:
		explicit IColorTarget(API::Texture& col) : _color(col) {};

		GET(API::Texture&, Color, _color);
	 private:
		API::Texture& _color;
	};
}