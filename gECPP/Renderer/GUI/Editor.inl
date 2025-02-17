//
// Created by scion on 2/13/2025.
//

#pragma once

#include "Editor.h"
#include <IMGUI/imgui.h>
#include <IMGUI/imgui_stdlib.h>

#define GE_SWITCH_TYPE(TYPE) if constexpr(std::is_same_v<RAW_T, TYPE>)
#define IM_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL ImGuiDataType IMType<TYPE> = ENUM;

namespace gE
{
    IM_TYPE_(i8, ImGuiDataType_S8);
    IM_TYPE_(u8, ImGuiDataType_U8);
    IM_TYPE_(i16, ImGuiDataType_S16);
    IM_TYPE_(u16, ImGuiDataType_U16);
    IM_TYPE_(i32, ImGuiDataType_S32);
    IM_TYPE_(u32, ImGuiDataType_U32);
    IM_TYPE_(i64, ImGuiDataType_S64);
    IM_TYPE_(u64, ImGuiDataType_U64);
    IM_TYPE_(float, ImGuiDataType_Float);
    IM_TYPE_(double, ImGuiDataType_Double);

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

        if constexpr (isConst)
            ImGui::BeginDisabled();

        ImGui::PushItemWidth(-1);

        bool changed = false;
        if constexpr(std::is_scalar_v<RAW_T> && !isPointer && !std::is_same_v<RAW_T, bool>)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<RAW_T>>);
            constexpr ImGuiDataType type = IMType<RAW_T>;

            switch(settings.ViewMode)
            {
            case ScalarViewMode::Slider:
                changed = ImGui::SliderScalar(label.c_str(), type, t, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
                break;
            case ScalarViewMode::Input:
                changed = ImGui::InputScalar(label.c_str(), type, t, &settings.Step, nullptr, nullptr, GE_EDITOR_INPUT_FLAGS);
                break;
            case ScalarViewMode::Drag:
            default:
                changed = ImGui::DragScalar(label.c_str(), type, t, (float) settings.Step, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
            }

            if constexpr(!isConst)
                *t = glm::clamp(*t, settings.Minimum, settings.Maximum);

            if(!settings.Tooltip.empty() && ImGui::IsItemHovered(GE_EDITOR_TOOLTIP_FLAGS))
                ImGui::SetTooltip(settings.Tooltip.data());
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
            const std::string_view type = t->GetType() ? t->GetType()->Name : "IReflectable (NO TYPE INFO)";

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

        if constexpr (isConst)
            ImGui::EndDisabled();

        ImGui::PopItemWidth();
        return changed;
    }

    template <class T, glm::length_t COMPONENT_COUNT>
    bool Editor::DrawField(const ScalarField<T>& settings, glm::vec<COMPONENT_COUNT, T>& t, u8 depth)
    {
        constexpr ImGuiDataType type = IMType<T>;

        const std::string label = std::format("##{}", settings.Name);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        /*if constexpr (CONST)
            ImGui::BeginDisabled();*/

        ImGui::PushItemWidth(-1);

        bool changed;
        switch(settings.ViewMode)
        {
        case ScalarViewMode::Slider:
            changed = ImGui::SliderScalarN(label.c_str(), type, &t, COMPONENT_COUNT, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
            break;
        case ScalarViewMode::Input:
            changed = ImGui::InputScalarN(label.c_str(), type, &t, COMPONENT_COUNT, &settings.Step, nullptr, nullptr, GE_EDITOR_INPUT_FLAGS);
            break;
        case ScalarViewMode::Drag:
        default:
            changed = ImGui::DragScalarN(label.c_str(), type, &t, COMPONENT_COUNT, (float) settings.Step, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
        }

        /*if constexpr(!CONST)
            t = glm::clamp(t, settings.Minimum, settings.Maximum);*/

        if(!settings.Tooltip.empty() && ImGui::IsItemHovered(GE_EDITOR_TOOLTIP_FLAGS))
            ImGui::SetTooltip(settings.Tooltip.data());

        /*if constexpr (CONST)
            ImGui::EndDisabled();*/

        ImGui::PopItemWidth();

        return changed;
    }

    template <class T, glm::length_t COMPONENT_COUNT>
    bool Editor::DrawField(const ScalarField<T>& settings, glm::vec<COMPONENT_COUNT, T>* t, u8 depth)
    {
        return DrawField(settings, *t, depth);
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