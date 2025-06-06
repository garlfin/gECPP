//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <Window.h>
#include "Window.h"
#include <Core/Converter/MeshLoader.h>
#include <Core/Converter/PVR.h>
#include <Vendor/IMGUI/imgui_internal.h>
#include <Vendor/SDL3/SDL_dialog.h>

#if defined(GE_ENABLE_EDITOR) && !defined(GE_ENABLE_IMGUI)
    #error IMGUI not enabled.
#endif

#ifdef GE_ENABLE_EDITOR
namespace gE::Editor
{
    Window::Window(Editor* editor, const std::string& name, ImGuiWindowFlags flags) :
        _editor(editor),
        _flags(flags),
        _name(name)
    {

    }

    gE::Window& Window::GetWindow() const
    {
        return _editor->GetWindow();
    }

    void Window::OnEditorGUI()
    {
        if(GetWindow().GetKeyboard().GetShortcut(_shortcut)) _isOpen = true;

        if(!_isOpen) return;

        if(ImGui::Begin(_name.c_str(), &_isOpen, _flags))
            IOnEditorGUI();
        ImGui::End();
    }

    EditorLog::EditorLog(Editor* editor) : Window(editor, "Console", ImGuiWindowFlags_MenuBar)
    {
        SetShortcut({ KeyModifier::LControl, KeyModifier::None, Key::L });
    }

    void EditorLog::IOnEditorGUI()
    {
        GE_ASSERTM(ImGui::GetCurrentContext(), "NO ACTIVE CONTEXT!");

        if(ImGui::Begin("Console", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if(ImGui::Button("Clear"))
                Log::Clear();

            ImGui::BeginMenuBar();
            ImGui::EndMenuBar();

            ImGui::BeginChild("log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            {
                ImGui::TextUnformatted(Log::Get().data());
                if(Log::Get().size() != _oldLogSize)
                    ImGui::SetScrollHereY(1.f);
                _oldLogSize = Log::Get().size();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    Editor::Editor(gE::Window* window) :
        _window(window),
        _log(this),
        _assetInspector(this),
        _assetManager(this, &_assetInspector),
        _entityInspector(this),
        _entityHierarchy(this, &_entityInspector),
        _camera(window)
    {
    }

    void Editor::OnGUI()
    {
        KeyboardState& keyboard = _window->GetKeyboard();

        if(!keyboard.GetIsFocused() && _isRunning && keyboard.GetKey(Key::F11) == KeyState::Pressed)
            _isOpen = !_isOpen;

        _window->SetViewport(Viewport(_window->GetSize(), DEFAULT));

        if(!_isOpen) return;

        ImGui::DockSpaceOverViewport(0, nullptr);

        ImGui::SetNextWindowSizeConstraints(ImVec2(256, 256), ImVec2(FLT_MAX, FLT_MAX));
        ImGui::Begin("TopBar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if(ImGui::Button(_isRunning ? "Pause" : "Play"))
            _isRunning = !_isRunning;

        const Size2D size = (Size2D) ImGui::GetContentRegionAvail();
        _window->SetViewport(Viewport(size));

        ImGui::Image((ImTextureID) _viewportTexture, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        _log.OnEditorGUI();

        _entityHierarchy.OnEditorGUI();
        _entityInspector.OnEditorGUI();

        _assetManager.OnEditorGUI();
        _assetInspector.OnEditorGUI();
    }

    bool Editor::OnRender()
    {
        _viewportTexture = &_window->GetCameras().GetCurrentCamera()->GetColor();
        if(!_isRunning)
        {
            _viewportTexture = &_camera.GetColor();
            _camera.GetCamera().Resize(_window->GetViewport().Size);
            _camera.GetCamera().OnRender(0.f, nullptr);
        }

        return _isRunning;
    }
}
#endif
