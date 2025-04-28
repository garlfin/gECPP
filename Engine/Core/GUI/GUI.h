//
// Created by scion on 2/5/2025.
//

#pragma once

#include <Prototype.h>
#include <Component/Camera/RenderTarget.h>
#include <Graphics/API/GL/Shader/Shader.h>
#include <Graphics/Buffer/FrameBuffer.h>
#include <Graphics/Texture/Texture.h>
#include <IMGUI/imgui.h>

namespace gE
{
    GLOBAL GPU::Texture GUIColorFormat = []
    {
        GPU::Texture tex;
        tex.Format = GL_RGBA8;
        tex.WrapMode = GPU::WrapMode::Clamp;
        tex.MipCount = 1;
        return tex;
    }();

#ifdef GE_ENABLE_IMGUI
    GLOBAL GPU::Texture IMGUIFontFormat = []
    {
        GPU::Texture tex;
        tex.Format = GL_R8;
        tex.WrapMode = GPU::WrapMode::Border;
        tex.MipCount = 1;
        tex.Filter = GPU::FilterMode::Linear;
        return tex;
    }();
#endif

    class GUIManager
    {
    public:
        explicit GUIManager(Window* window);

        GET_CONST(const API::Texture2D&, Color, *_color);

        void BeginGUI();
        void EndGUI();
        void OnRender(const ImDrawData*) const;

        void Resize(Size2D size);

        ~GUIManager();

    private:
        Window* _window;

        API::Framebuffer _framebuffer;
        Attachment<API::Texture2D, GL_COLOR_ATTACHMENT0> _color;

#ifdef GE_ENABLE_IMGUI
        API::Shader _imShader;
        API::Texture2D _imFontTex;

        static void SetStyle();
#endif
    };
}
