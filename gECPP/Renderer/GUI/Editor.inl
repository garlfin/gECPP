//
// Created by scion on 2/13/2025.
//

#pragma once

#include "Editor.h"
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_stdlib.h>

#define GE_SWITCH_TYPE(TYPE) if constexpr(std::is_same_v<RAW_T, TYPE>)

namespace gE
{
    template <class T, class SETTINGS_T>
    bool Editor::DrawField(const SETTINGS_T& settings, T& t_, u8 depth)
    {
        using RAW_T = std::remove_cvref_t<std::remove_pointer_t<T>>;
        constexpr bool isPointer = std::is_pointer_v<std::remove_cvref_t<T>>;
        constexpr bool isConst = std::is_const_v<std::remove_pointer_t<T>>;

        RAW_T* t = nullptr;

        if constexpr(!isPointer)
            t = (RAW_T*) &t_;
        else
            t = (RAW_T*) t_;

        const std::string label = std::format("##{}", settings.Name);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        if(!t)
        {
            ImGui::TextUnformatted(std::format("{}", (void*) t).c_str());
            return false;
        }

        if(isConst)
            ImGui::BeginDisabled();

        ImGui::PushItemWidth(-1);

        bool changed = false;
        if constexpr(std::is_scalar_v<RAW_T> && !isPointer && !std::is_same_v<RAW_T, bool>)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<RAW_T>>);
            ImGuiDataType type;

            GE_SWITCH_TYPE(i8) type = ImGuiDataType_S8;
            GE_SWITCH_TYPE(u8) type = ImGuiDataType_U8;
            GE_SWITCH_TYPE(i16) type = ImGuiDataType_S16;
            GE_SWITCH_TYPE(u16) type = ImGuiDataType_U16;
            GE_SWITCH_TYPE(i32) type = ImGuiDataType_S32;
            GE_SWITCH_TYPE(u32) type = ImGuiDataType_U32;
            GE_SWITCH_TYPE(i64) type = ImGuiDataType_S64;
            GE_SWITCH_TYPE(u64) type = ImGuiDataType_U64;
            GE_SWITCH_TYPE(float) type = ImGuiDataType_Float;
            GE_SWITCH_TYPE(double) type = ImGuiDataType_Double;

            changed = ImGui::DragScalar(label.c_str(), type, t, (float) settings.Step, &settings.Minimum, &settings.Maximum, nullptr, ImGuiSliderFlags_AlwaysClamp);

            if(!settings.Tooltip.empty() && ImGui::IsItemHovered(GE_EDITOR_TOOLTIP_FLAGS))
                ImGui::SetTooltip(settings.Tooltip.data());
        }
        else GE_SWITCH_TYPE(glm::vec2)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<float>>);
            changed = ImGui::DragFloat2(label.c_str(), (float*) t, (float) settings.Step, settings.Minimum, settings.Maximum, nullptr, ImGuiSliderFlags_AlwaysClamp);
        }
        else GE_SWITCH_TYPE(glm::vec3)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<float>>);
            changed = ImGui::DragFloat3(label.c_str(), (float*) t, (float) settings.Step, settings.Minimum, settings.Maximum, nullptr, ImGuiSliderFlags_AlwaysClamp);
        }
        else GE_SWITCH_TYPE(glm::vec4)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<float>>);
            changed = ImGui::DragFloat4(label.c_str(), (float*) t, (float) settings.Step, settings.Minimum, settings.Maximum, nullptr, ImGuiSliderFlags_AlwaysClamp);
        }
        else GE_SWITCH_TYPE(bool)
        {
            T temp = *t;
            if constexpr(isConst)
                ImGui::Checkbox(label.c_str(), &temp);
            else
            {
                ImGui::Checkbox(label.c_str(), t);
                changed = temp == *t;
            }
        }
        else GE_SWITCH_TYPE(std::string)
        {
            changed = ImGui::InputText(label.c_str(), t);
        }
        else if constexpr(std::is_base_of_v<IReflectable, RAW_T>)
        {
            const std::string_view type = t->GetType() ? t->GetType()->Name : "IReflectable";

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if(depth < 1) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

            if(ImGui::TreeNodeEx(std::format("{} ({})", type, (void*) t).c_str(), nodeFlags))
            {
                t->OnEditorGUI(depth + 1);
                ImGui::TreePop();
            }
        }
        else if constexpr(isPointer)
            ImGui::TextUnformatted(std::format("{}", (void*) t_).c_str());
        else
            static_assert(false);

        if(isConst)
            ImGui::EndDisabled();

        ImGui::PopItemWidth();
        return changed;
    }

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool Editor::DrawField(const SETTINGS_T& settings, OWNER_T& owner, u8 depth, GetterFunction<OUT_T, OWNER_T> get, SetterFunction<IN_T, OWNER_T> set)
    {
        using T = std::remove_cvref_t<IN_T>;
        static_assert(std::is_same_v<T, std::remove_cvref_t<OUT_T>>);

        GE_ASSERTM(get && set, "CANNOT HAVE NULL GETTER SETTER!");

        T old = (owner.*get)();
        T result = old;

        const bool edited = DrawField(settings, result, depth);
        if(edited)
            (owner.*set)(result);

        return edited;
    }

}

#undef GE_SWITCH_TYPE