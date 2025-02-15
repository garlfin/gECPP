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
        if(!ImGui::BeginTable(std::format("##table_{}", settings.Name).c_str(), 2, GE_EDITOR_TABLE_FLAGS))
            return false;

        using RAW_T = std::remove_cvref_t<std::remove_pointer_t<T>>;
        constexpr bool isPointer = std::is_pointer_v<std::remove_cvref_t<T>>;
        constexpr bool isConst = std::is_const_v<std::remove_pointer_t<T>>;

        RAW_T* t = nullptr;

        if constexpr(!isPointer)
            t = (RAW_T*) &t_;
        else
            t = (RAW_T*) t_;

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
        if constexpr(isConst) flags |= ImGuiInputTextFlags_ReadOnly;

        const std::string labelStr = std::format("##{}", settings.Name);
        const char* label = labelStr.c_str();

        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(settings.Name.data());

        ImGui::TableSetColumnIndex(1);

        if(!t)
        {
            ImGui::TextUnformatted(std::format("{}", (void*) t).c_str());
            ImGui::EndTable();
            return false;
        }

        ImGui::PushItemWidth(-1);

        bool changed = false;
        if constexpr(std::is_scalar_v<RAW_T> && !isPointer && !std::is_same_v<RAW_T, bool>)
        {
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

            changed = ImGui::InputScalar(label, type, t, nullptr, nullptr, nullptr, flags);

            if(!settings.Tooltip.empty())
            {
                if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::SetTooltip(settings.Tooltip.data());
                }
            }
        }
        else GE_SWITCH_TYPE(bool)
        {
            T temp = *t;
            if constexpr(isConst)
                ImGui::Checkbox(label, &temp);
            else
            {
                ImGui::Checkbox(label, t);
                changed = temp == *t;
            }
        }
        else GE_SWITCH_TYPE(std::string)
        {
            changed = ImGui::InputText(label, t, flags);
        }
        else if constexpr(std::is_base_of_v<IReflectable, RAW_T>)
        {
            const std::string_view type = t->GetType() ? t->GetType()->Name : "IReflectable";

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanTextWidth;
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

        ImGui::PopItemWidth();
        ImGui::EndTable();
        return changed;
    }

    template <class T, class SETTINGS_T, class OWNER_T>
    bool Editor::DrawField(const SETTINGS_T& settings, OWNER_T& owner, u8 depth, GetterFunction<T, OWNER_T> get, SetterFunction<T, OWNER_T> set)
    {
        GE_ASSERTM(get && set, "CANNOT HAVE NULL GETTER SETTER!");

        T old = owner.*get();
        T result = old;

        const bool edited = DrawField(settings, result, depth);
        if(edited)
            owner.*set(result);

        return edited;
    }
}

#undef GE_SWITCH_TYPE