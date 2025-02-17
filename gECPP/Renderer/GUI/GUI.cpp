//
// Created by scion on 2/5/2025.
//

#include "GUI.h"

#include <GLM/gtc/matrix_transform.hpp>
#include <IMGUI/imgui_internal.h>
#include <IMGUI/backends/imgui_impl_sdl3.h>
#include <Window/Window.h>

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
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Resource/Font/SourceSans3-SemiBold.ttf", 32);
    io.FontDefault->Scale = 0.75;
    io.Fonts->GetTexDataAsAlpha8(&pixelData, &width, &height);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    SetStyle();

    auto fontFormat = GPU::Texture2D(IMGUIFontFormat, Size2D(width, height));
    fontFormat.Data.Data = Array(width * height, (const u8*) pixelData);
    fontFormat.Data.MipCount = 1;
    fontFormat.Data.PixelFormat = GL_RED;
    fontFormat.Data.PixelType = GL_UNSIGNED_BYTE;

    PlacementNew(_imFont, window, move(fontFormat));
    PlacementNew(_imShader, window, GPU::Shader("Resource/Shader/GUI/IMGUI.vert", "Resource/Shader/GUI/IMGUI.frag"));
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
    glViewport(0, 0, _color->GetSize().x, _color->GetSize().y);
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

// https://github.com/adia-dev
void gE::GUIManager::SetStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base Colors
    ImVec4 bgColor = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
    ImVec4 lightBgColor = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
    ImVec4 panelColor = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
    ImVec4 panelHoverColor = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
    ImVec4 panelActiveColor = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
    ImVec4 textColor = ImVec4(0.86f, 0.87f, 0.88f, 1.00f);
    ImVec4 textDisabledColor = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    ImVec4 borderColor = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);

    // Text
    colors[ImGuiCol_Text] = textColor;
    colors[ImGuiCol_TextDisabled] = textDisabledColor;

    // Windows
    colors[ImGuiCol_WindowBg] = bgColor;
    colors[ImGuiCol_ChildBg] = bgColor;
    colors[ImGuiCol_PopupBg] = bgColor;
    colors[ImGuiCol_Border] = borderColor;
    colors[ImGuiCol_BorderShadow] = borderColor;

    // Headers
    colors[ImGuiCol_Header] = panelColor;
    colors[ImGuiCol_HeaderHovered] = panelHoverColor;
    colors[ImGuiCol_HeaderActive] = panelActiveColor;

    // Buttons
    colors[ImGuiCol_Button] = panelColor;
    colors[ImGuiCol_ButtonHovered] = panelHoverColor;
    colors[ImGuiCol_ButtonActive] = panelActiveColor;

    // Frame BG
    colors[ImGuiCol_FrameBg] = lightBgColor;
    colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
    colors[ImGuiCol_FrameBgActive] = panelActiveColor;

    // Tabs
    colors[ImGuiCol_Tab] = panelColor;
    colors[ImGuiCol_TabHovered] = panelHoverColor;
    colors[ImGuiCol_TabActive] = panelActiveColor;
    colors[ImGuiCol_TabUnfocused] = panelColor;
    colors[ImGuiCol_TabUnfocusedActive] = panelHoverColor;

    // Title
    colors[ImGuiCol_TitleBg] = bgColor;
    colors[ImGuiCol_TitleBgActive] = bgColor;
    colors[ImGuiCol_TitleBgCollapsed] = bgColor;

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = bgColor;
    colors[ImGuiCol_ScrollbarGrab] = panelColor;
    colors[ImGuiCol_ScrollbarGrabHovered] = panelHoverColor;
    colors[ImGuiCol_ScrollbarGrabActive] = panelActiveColor;

    // Checkmark
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

    // Slider
    colors[ImGuiCol_SliderGrab] = panelHoverColor;
    colors[ImGuiCol_SliderGrabActive] = panelActiveColor;

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = panelColor;
    colors[ImGuiCol_ResizeGripHovered] = panelHoverColor;
    colors[ImGuiCol_ResizeGripActive] = panelActiveColor;

    // Separator
    colors[ImGuiCol_Separator] = borderColor;
    colors[ImGuiCol_SeparatorHovered] = panelHoverColor;
    colors[ImGuiCol_SeparatorActive] = panelActiveColor;

    // Plot
    colors[ImGuiCol_PlotLines] = textColor;
    colors[ImGuiCol_PlotLinesHovered] = panelActiveColor;
    colors[ImGuiCol_PlotHistogram] = textColor;
    colors[ImGuiCol_PlotHistogramHovered] = panelActiveColor;

    // Text Selected BG
    colors[ImGuiCol_TextSelectedBg] = panelActiveColor;

    // Modal Window Dim Bg
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.105f, 0.11f, 0.5f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = panelColor;
    colors[ImGuiCol_TableBorderStrong] = borderColor;
    colors[ImGuiCol_TableBorderLight] = borderColor;
    colors[ImGuiCol_TableRowBg] = bgColor;
    colors[ImGuiCol_TableRowBgAlt] = lightBgColor;

    // Styles
    style.FrameBorderSize = 0.0f;
    style.FrameRounding = 5.0f;
    style.WindowBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabMinSize = 7.0f;
    style.GrabRounding = 5.0f;
    style.TabBorderSize = 0.0f;
    style.TabRounding = 5.0f;
    style.WindowRounding = 5.0f;

    // Reduced Padding and Spacing
    style.WindowPadding = ImVec2(5.0f, 5.0f);
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
}
