//
// Created by scion on 2/5/2025.
//

#include "GUI.h"

#include <IMGUI/backends/imgui_impl_sdl3.h>
#include <Window/Window.h>
#include <GLM/gtc/matrix_transform.hpp>

#include "IMGUI/imgui_internal.h"
#include "Utility/Log.h"

gE::GUIManager::GUIManager(Window* window) :
    _window(window),
    _framebuffer(window),
    _color(_framebuffer, GPU::Texture2D(GUIColorFormat, window->GetSize()))
{
#ifdef GE_ENABLE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForOther(window->GetSDLWindow());

    unsigned char* pixelData;
    int width, height;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->GetTexDataAsAlpha8(&pixelData, &width, &height);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

void gE::GUIManager::BeginGUI()
{
    const bool active = _window->GetMouse().GetIsEnabled();

#ifdef GE_ENABLE_IMGUI
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    if(!active) ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;

    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
#endif

    _framebuffer.Bind();
    glClear(GL_COLOR_BUFFER_BIT);
}

void gE::GUIManager::EndGUI()
{
#ifdef GE_ENABLE_IMGUI
    ImGui::Render();
    OnRender(ImGui::GetDrawData());
#endif
}

#ifdef GE_ENABLE_IMGUI
// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp
void gE::GUIManager::OnRender(const ImDrawData* draw) const
{
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

    vaoFormat.Buffers[0] = GPU::Buffer<>(sizeof(ImDrawVert) * vertSize, nullptr, sizeof(ImDrawVert), false);
    vaoFormat.Buffers[0].UsageHint = GPU::BufferUsageHint::Dynamic;

    vaoFormat.TriangleBuffer = GPU::Buffer<>(sizeof(ImDrawIdx) * triSize, nullptr, sizeof(ImDrawIdx), false);
    vaoFormat.TriangleBuffer.UsageHint = GPU::BufferUsageHint::Dynamic;
    vaoFormat.TriangleFormat = GLType<ImDrawIdx>;

    API::IndexedVAO vao(_window, std::move(vaoFormat));

    DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

    const float right = draw->DisplayPos.x + draw->DisplaySize.x;
    const float bottom = draw->DisplayPos.y + draw->DisplaySize.y;

    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    buffers.Camera.Projection = glm::ortho(draw->DisplayPos.x, right, draw->DisplayPos.y, bottom);
    buffers.UpdateCamera(sizeof(glm::mat4), offsetof(GPU::Camera, Projection));

    _imShader.Bind();

    const ImVec2 clipOffset = draw->DisplayPos;
    int height = (int) draw->DisplaySize.y;

    for(const ImDrawList* cmdList : draw->CmdLists)
    {
        vao.UpdateBufferDirect(0, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.size_in_bytes());
        vao.UpdateIndicesDirect(cmdList->IdxBuffer.Data, cmdList->IdxBuffer.size_in_bytes());

        for(const ImDrawCmd& drawCall : cmdList->CmdBuffer)
        {
            ImVec2 clipMin(drawCall.ClipRect.x - clipOffset.x, drawCall.ClipRect.y - clipOffset.y);
            ImVec2 clipMax(drawCall.ClipRect.z - clipOffset.x, drawCall.ClipRect.w - clipOffset.y);
            if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                continue;

            glScissor((int)clipMin.x, (int)((float)height - clipMax.y), (int)(clipMax.x - clipMin.x), (int)(clipMax.y - clipMin.y));

            _imShader.SetUniform(1, _imFont, 0);
            vao.DrawDirect(drawCall.ElemCount / 3, drawCall.IdxOffset);
        }
    }

    glDisable(GL_SCISSOR_TEST);
}
#endif

gE::GUIManager::~GUIManager()
{
#ifdef GE_ENABLE_IMGUI
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif
}