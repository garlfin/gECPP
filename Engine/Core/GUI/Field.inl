//
// Created by scion on 2/13/2025.
//

#pragma once

#include "Field.h"
#include "Editor/Settings.h"

#include <Vendor/IMGUI/imgui.h>

#define GE_SWITCH_TYPE(TYPE) if constexpr(std::is_same_v<RAW_T, TYPE>)
#define IM_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL ImGuiDataType IMType<TYPE> = ENUM;

#ifdef GE_ENABLE_IMGUI
namespace gE::Editor
{
    template<class BASE_T, class T, class SETTINGS_T>
    bool DragDropCompare(ImGuiPayload& payload, const DragDropField<BASE_T, T, SETTINGS_T>* field)
    {
        const bool isType = payload.IsDataType(field->Type.c_str());
        const Reference<BASE_T>& data = **(const Reference<BASE_T>**) payload.Data;

        return isType && field->Acceptor(data, field->UserData);
    }

    template<class BASE_T, class T, class SETTINGS_T> requires IsDragDroppable<BASE_T, T>
    bool DrawField(const DragDropField<BASE_T, T, SETTINGS_T>& settings, Reference<T>& ref, u8 depth)
    {
        using RAW_T = std::remove_const_t<T>;
        constexpr bool isConst = std::is_const_v<T>;

        GE_ASSERT(!settings.Type.empty());
        GE_ASSERT(settings.Acceptor);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if(depth < 1) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        const bool open = ImGui::TreeNodeEx(std::format("{}##{}", GetLabel<T>(ref.GetPointer()), settings.Name).c_str(), nodeFlags);

        bool changed = false;
        if(ImGui::BeginDragDropTarget())
        {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((IMGUI_ACCEPT_PAYLOAD_FUNC) DragDropCompare<BASE_T, T, SETTINGS_T>, &settings))
            {
                const Reference<T>& newReference = **(const Reference<T>**) payload->Data;
                if(ref != newReference)
                {
                    changed = true;
                    ref = newReference;
                }
            }
            ImGui::EndDragDropTarget();
        }

        if(open)
        {
            if constexpr (isConst)
                ImGui::BeginDisabled();

            // not good if const, runs non-const function on const object
            if(ref) ((RAW_T&) *ref).OnEditorGUI(depth + 1);

            if constexpr (isConst)
                ImGui::EndDisabled();

            ImGui::TreePop();
        }

        return changed;
    }

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool DrawField(const SETTINGS_T& settings, OWNER_T& owner, u8 depth, GetterFunction<OUT_T, OWNER_T> get, SetterFunction<IN_T, OWNER_T> set)
    {
        using T = std::remove_cvref_t<IN_T>;
        static_assert(std::is_same_v<T, std::remove_cvref_t<OUT_T>>);

        GE_ASSERTM(get && set, "CANNOT HAVE NULL GETTER SETTER!");

        T result = (owner.*get)();
        const bool changed = DrawField(settings, result, depth);
        if(changed)
            (owner.*set)(std::move(result));

        return changed;
    }

}
#endif

#undef GE_SWITCH_TYPE
#undef IM_TYPE_