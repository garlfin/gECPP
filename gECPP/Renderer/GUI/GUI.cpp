//
// Created by scion on 2/5/2025.
//

#include "GUI.h"

#include <IMGUI/backends/imgui_impl_glfw.h>
#include <Window/Window.h>
#include <GLM/gtc/matrix_transform.hpp>

#include "IMGUI/imgui_internal.h"

gE::GUIManager::GUIManager(Window* window) :
    _window(window),
    _framebuffer(window),
    _color(_framebuffer, GPU::Texture2D(GUIColorFormat, window->GetSize()))
{
#ifdef GE_ENABLE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOther(window->GLFWWindow(), true);

    ImGui::GetCurrentContext()->DebugLogFlags &= ~ImGuiDebugLogFlags_OutputToTTY;

    unsigned char* pixelData;
    int width, height;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->GetTexDataAsAlpha8(&pixelData, &width, &height);

    auto fontFormat = GPU::Texture2D(IMGUIFontFormat, TextureSize2D(width, height));
    fontFormat.Data.Data = Array(width * height, (const u8*) pixelData);
    fontFormat.Data.MipCount = 1;
    fontFormat.Data.PixelFormat = GL_RED;
    fontFormat.Data.PixelType = GL_UNSIGNED_BYTE;

    SAFE_CONSTRUCT_NAMESPACE(_imFont, API, Texture2D, window, move(fontFormat))
    SAFE_CONSTRUCT_NAMESPACE(_imShader, API, Shader, window, GPU::Shader("Resource/Shader/GUI/IMGUI.vert", "Resource/Shader/GUI/IMGUI.frag"));

    io.Fonts->SetTexID(_imFont.Get());
#endif
}

void gE::GUIManager::OnRender(float delta)
{
    #ifdef GE_ENABLE_IMGUI
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
        if(!_window->GetCursorEnabled()) ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    #endif

    ImGui::ShowDebugLogWindow();

    _framebuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    #ifdef GE_ENABLE_IMGUI
        ImGui::Render();
        OnRender(ImGui::GetDrawData());
    #endif
}

#ifdef GE_ENABLE_IMGUI
// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp
void gE::GUIManager::OnRender(const ImDrawData* draw)
{
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GE_ASSERTM(draw, "NO DRAW DATA SUPPLIED.");
    GE_ASSERTM(draw->Valid, "INVALID DRAW.");

    size_t vertSize = 0;
    size_t triSize = 0;
    for(const ImDrawList* drawList : draw->CmdLists)
    {
        vertSize = std::max<size_t>(vertSize, drawList->VtxBuffer.size());
        triSize = std::max<size_t>(triSize, drawList->IdxBuffer.size());
    }

    if(!triSize || !vertSize) return;

    GPU::IndexedVAO vaoFormat = DEFAULT;
    vaoFormat.Counts.BufferCount = 1;
    vaoFormat.Counts.FieldCount = 3;

    vaoFormat.Fields[0] = GPU::VertexField("POS", GL_FLOAT, false, 0, 0, 2, 0);
    vaoFormat.Fields[1] = GPU::VertexField("UV", GL_FLOAT, false, 0, 1, 2, offsetof(ImDrawVert, uv));
    vaoFormat.Fields[2] = GPU::VertexField("COL", GL_UNSIGNED_BYTE, true, 0, 2, 4, offsetof(ImDrawVert, col));

    vaoFormat.Buffers[0] = GPU::Buffer<u8>(sizeof(ImDrawVert) * vertSize, nullptr, sizeof(ImDrawVert), false);
    vaoFormat.Buffers[0].UsageHint = GPU::BufferUsageHint::Dynamic;

    vaoFormat.TriangleBuffer = GPU::Buffer<u8>(sizeof(ImDrawIdx) * triSize, nullptr, sizeof(ImDrawIdx), false);
    vaoFormat.TriangleBuffer.UsageHint = GPU::BufferUsageHint::Dynamic;
    vaoFormat.TriangleFormat = GLType<ImDrawIdx>;

    API::IndexedVAO vao(_window, std::move(vaoFormat));

    DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

    const float right = draw->DisplayPos.x + draw->DisplaySize.x;
    const float bottom = draw->DisplayPos.y + draw->DisplaySize.y;

    buffers.Camera.Projection = glm::ortho(draw->DisplayPos.x, right, draw->DisplayPos.y, bottom);
    buffers.UpdateCamera(sizeof(glm::mat4), offsetof(GPU::Camera, Projection));

    _imShader.Bind();

    const ImVec2 clip_off = draw->DisplayPos;
    int fb_height = (int) draw->DisplaySize.y;

    for(const ImDrawList* cmdList : draw->CmdLists)
    {
        vao.UpdateBufferDirect(0, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.size_in_bytes());
        vao.UpdateIndicesDirect(cmdList->IdxBuffer.Data, cmdList->IdxBuffer.size_in_bytes());

        for(const ImDrawCmd& drawCall : cmdList->CmdBuffer)
        {
            ImVec2 clip_min(drawCall.ClipRect.x - clip_off.x, drawCall.ClipRect.y - clip_off.y);
            ImVec2 clip_max(drawCall.ClipRect.z - clip_off.x, drawCall.ClipRect.w - clip_off.y);
            if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                continue;

            glScissor((int)clip_min.x, (int)((float)fb_height - clip_max.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y));

            _imShader.SetUniform(1, _imFont, 0);
            vao.DrawDirect(drawCall.ElemCount / 3, drawCall.IdxOffset);
        }
    }

    glDisable(GL_SCISSOR_TEST);
}

gE::GUIManager::~GUIManager()
{
#ifdef GE_ENABLE_IMGUI
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
}
#endif
