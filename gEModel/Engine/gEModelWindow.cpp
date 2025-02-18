//
// Created by scion on 11/14/2024.
//

#include "gEModelWindow.h"

#include <SDL3/SDL_dialog.h>
#include <IMGUI/imgui_stdlib.h>
#include <Core/GUI/Editor.h>

#include "Utility/MeshLoader.h"

namespace gE::gEModel
{
    CONSTEXPR_GLOBAL SDL_DialogFileFilter MODEL_FILTER { "3D Models", "dae;fbx;obj;gltf;glb" };
    CONSTEXPR_GLOBAL SDL_DialogFileFilter MODEL_OUTPUT_FILTER { "gEMesh", "mesh" };

    void FileCallbackInput(gEModelWindow* out, const char* const* files, int);
    void FileCallbackOutput(gEModelWindow* out, const char* const* files, int);

    void gEModelWindow::OnRender(float x)
    {
        GUI->BeginGUI();

        ImGui::Begin("##window", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

        ImGui::SetWindowSize((ImVec2) GetSize());
        ImGui::SetWindowPos(ImVec2(0, 0));

        // Input
        ImGui::TextUnformatted("Input");
        ImGui::SameLine();
        ImGui::SetCursorPosX(100.f);
        ImGui::InputText("##input_input", &_input);
        ImGui::SameLine();
        if(ImGui::Button("Browse##input"))
            SDL_ShowOpenFileDialog((SDL_DialogFileCallback) FileCallbackInput, this, nullptr, &MODEL_FILTER, 1, _input.c_str(), false);

        // Output
        /*ImGui::TextUnformatted("Output");
        ImGui::SameLine();
        ImGui::SetCursorPosX(100.f);
        ImGui::InputText("##input_output", &_output);
        ImGui::SameLine();
        if(ImGui::Button("Browse##output"))
            SDL_ShowSaveFileDialog((SDL_DialogFileCallback) FileCallbackOutput, this, nullptr, &MODEL_OUTPUT_FILTER, 1, _output.c_str());*/

        if(!_input.empty() && ImGui::Button("Initial Conversion"))
            ConvertFile(this, _input);

        if(_meshes)
            DrawField(ArrayField<Field>{}, _meshes, 0);

        ImGui::End();
        GUI->EndGUI();

        API::Framebuffer::Reset();
        glViewport(0, 0, GetSize().x, GetSize().y);
        Blit(GUI->GetColor());
    }

    void FileCallbackInput(gEModelWindow* out, const char* const* files, int)
    {
        GE_ASSERT(files);
        if(!files[0]) return;

        std::string path = std::string(files[0]);
        out->SetOutput(Path(path).replace_extension(".mesh").string());
        out->SetInput(move(path));
    }

    void FileCallbackOutput(gEModelWindow* out, const char* const* files, int)
    {
        GE_ASSERT(files);
        if(!files[0]) return;

        out->SetOutput(std::string(files[0]));
    }
}
