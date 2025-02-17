//
// Created by scion on 12/1/2023.
//

#pragma once

#include <Graphics/Buffer/FrameBuffer.h>
#include <Utility/RelativePointer.h>
#include <Renderer/Buffers.h>

namespace gE
{
    class Camera;

    template <class T, GLenum TARGET>
    class Attachment
    {
    public:
        template <typename... ARGS>
        explicit Attachment(API::Framebuffer& f, ARGS&&... args) : _texture(&f.GetWindow(), std::forward<ARGS>(args)...)
        {
            if constexpr (TARGET == GL_DEPTH_ATTACHMENT || TARGET == GL_DEPTH_STENCIL_ATTACHMENT)
                f.SetDepthAttachment(_texture);
            else
                f.SetAttachment(TARGET - GL_COLOR_ATTACHMENT0, _texture);
        }

        Attachment() = default;

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
        T _texture = DEFAULT;
    };

    class IRenderTarget
    {
    public:
        virtual ~IRenderTarget() = default;

        explicit IRenderTarget(Entity& owner, Camera& camera);

        GET(Camera&, Camera, *_camera);
        GET(Entity&, Owner, *_owner);
        GET(API::Framebuffer&, FrameBuffer, _frameBuffer);
        GET(Window&, Window, *_window);

        virtual bool Setup(float, Camera*) { return true; }

        virtual void RenderDependencies(float) {};
        virtual void RenderPass(float, Camera*) = 0;
        virtual void PostProcessPass(float) {};
        virtual void GetGPUCameraOverrides(GPU::Camera&) const {};
        // Matches textures to camera's size. (WILL NOT CHANGE THE CAMERA'S RESOLUTION)
        virtual void Resize() = 0;

        inline void Bind() const { _frameBuffer.Bind(); }

    private:
        RelativePointer<Camera> _camera;
        RelativePointer<Entity> _owner;
        API::Framebuffer _frameBuffer;
        Window* _window;
    };

    template <class T>
    class RenderTarget : public IRenderTarget
    {
    public:
        using CAMERA_T = T;
        using TEX_T = typename T::TEX_T;
        using IRenderTarget::IRenderTarget;

        GET(T&, Camera, (T&) IRenderTarget::GetCamera());
        GET_CONST(typename T::SIZE_T, Size, GetCamera().GetSize());
    };

    class IDepthTarget
    {
    public:
        IDepthTarget(Camera& camera, API::Texture& depth) : _camera(camera), _depth(depth) {};

        GET(API::Texture&, Depth, *_depth);
        GET(Camera&, Camera, *_camera);

    private:
        RelativePointer<Camera> _camera;
        RelativePointer<API::Texture> _depth;
    };

    template<class T>
    class DepthTarget : public IDepthTarget
    {
    public:
        using CAMERA_T = T;
        using TEX_T = typename T::TEX_T;

        DepthTarget(CAMERA_T& camera, TEX_T& depth) : IDepthTarget(camera, depth) {};

        GET(TEX_T&, Depth, (TEX_T&) IDepthTarget::GetDepth());
        GET(CAMERA_T&, Camera, (CAMERA_T&) IDepthTarget::GetCamera());
    };

    class IColorTarget
    {
    public:
        IColorTarget(Camera& camera, API::Texture& col) : _camera(camera), _color(col) {};

        GET(API::Texture&, Color, *_color);
        GET(Camera&, Camera, *_camera);

    private:
        RelativePointer<Camera> _camera;
        RelativePointer<API::Texture> _color;
    };

    template<class T>
    class ColorTarget : public IColorTarget
    {
    public:
        using CAMERA_T = T;
        using TEX_T = typename T::TEX_T;

        ColorTarget(CAMERA_T& camera, TEX_T& color) : IColorTarget(camera, color) {};

        GET(TEX_T&, Color, (TEX_T&) IColorTarget::GetColor());
        GET(CAMERA_T&, Camera, (CAMERA_T&) IColorTarget::GetCamera());
    };
}
