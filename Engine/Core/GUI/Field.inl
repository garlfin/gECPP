//
// Created by scion on 2/13/2025.
//

#pragma once

#include "Field.h"
#include "Editor/Settings.h"

#include <Core/Serializable/Asset.h>
#include <Vendor/IMGUI/imgui.h>
#include <Vendor/IMGUI/imgui_stdlib.h>

#define GE_SWITCH_TYPE(TYPE) if constexpr(std::is_same_v<RAW_T, TYPE>)
#define IM_TYPE_(TYPE, ENUM) template<> CONSTEXPR_GLOBAL ImGuiDataType IMType<TYPE> = ENUM;

#ifdef GE_ENABLE_IMGUI
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
    bool DrawField(const SETTINGS_T& settings, T& t, u8 depth)
    {
        using RAW_T = std::remove_cvref_t<T>;
        constexpr bool isConst = std::is_const_v<T>;
        constexpr bool isReflectable = std::is_base_of_v<IReflectable, RAW_T>;

        const std::string label = std::format("##{}", settings.Name);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        if constexpr (isConst && !isReflectable)
            ImGui::BeginDisabled();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());

        bool changed = false;
        if constexpr(std::is_enum_v<RAW_T>)
        {
            const auto& eType = settings.Type;
            const std::string preview = eType.ToString(t);

            if(eType.Type == EnumType::Normal)
            {
                if(ImGui::BeginCombo(label.c_str(), preview.c_str()))
                {
                    if constexpr(!isConst)
                    {
                        for(const auto& e : eType.Enums)
                        {
                            const bool selected = t == e.first;
                            if(ImGui::Selectable(e.second.data(), selected))
                            {
                                if(!selected)
                                    changed = true;
                                t = e.first;
                            }
                            if(selected)
                                ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
            else
            {

            }
        }
        else GE_SWITCH_TYPE(bool)
        {
            T temp = t;
            if constexpr(isConst)
                ImGui::Checkbox(label.c_str(), &temp);
            else
            {
                ImGui::Checkbox(label.c_str(), &t);
                changed = temp == t;
            }
        }
        else GE_SWITCH_TYPE(std::string)
        {
            changed = ImGui::InputText(label.c_str(), (std::string*) &t);
        }
        else GE_SWITCH_TYPE(std::span<char>)
        {
            changed = ImGui::InputText(label.c_str(), (char*) t.data(), t.size());
        }
        else if constexpr(std::is_scalar_v<RAW_T> && !std::is_same_v<RAW_T, bool>)
        {
            static_assert(std::is_same_v<SETTINGS_T, ScalarField<RAW_T>>);
            constexpr ImGuiDataType type = IMType<RAW_T>;

            switch(settings.ViewMode)
            {
            case ScalarViewMode::Slider:
                changed = ImGui::SliderScalar(label.c_str(), type, (void*) &t, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
                break;
            case ScalarViewMode::Input:
                changed = ImGui::InputScalar(label.c_str(), type, (void*) &t, &settings.Step);
                break;
            case ScalarViewMode::Drag:
            default:
                changed = ImGui::DragScalar(label.c_str(), type, (void*) &t, (float) settings.Step, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
            }

            if constexpr(!isConst)
                t = glm::clamp(t, settings.Minimum, settings.Maximum);

            if(!settings.Tooltip.empty() && ImGui::IsItemHovered(GE_EDITOR_TOOLTIP_FLAGS))
                ImGui::SetTooltip(settings.Tooltip.data());
        }
        else if constexpr(isReflectable)
        {
            std::string_view type = "IReflectable (NO TYPE INFO)";
            if constexpr(requires(RAW_T e) { e.GetType(); })
                if(const auto* reflectedType = t.GetType())
                    type = reflectedType->Name;

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if(depth < 1) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

            if(ImGui::TreeNodeEx(std::format("{} ({})", type, (void*) &t).c_str(), nodeFlags))
            {
                if constexpr (isConst)
                    ImGui::BeginDisabled();

                // not good if const, runs non-const function on const object
                ((RAW_T&) t).OnEditorGUI(depth + 1);

                if constexpr (isConst)
                    ImGui::EndDisabled();

                ImGui::TreePop();
            }
        }
        else
            static_assert(false);

        if constexpr (isConst && !isReflectable)
            ImGui::EndDisabled();

        return changed;
    }

    template <class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T& settings, T* t, u8 depth)
    {
        const std::string label = std::format("##{}", settings.Name);
        std::string ptr = std::format("{}", (void*) t);

        if(t)
            return DrawField(settings, *t, depth);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());

        ImGui::BeginDisabled();
        ImGui::InputText(label.c_str(), &ptr, ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();
        return false;
    }

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>& settings, glm::vec<COMPONENT_COUNT, T>& vec, u8 depth)
    {
        constexpr ImGuiDataType type = IMType<T>;
        using RAW_T = glm::vec<COMPONENT_COUNT, T>;

        const std::string label = std::format("##{}", settings.Name);

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        /*if constexpr (CONST)
            ImGui::BeginDisabled();*/

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());

        bool changed = false;
        if(settings.ViewMode == ScalarViewMode::ColorPicker)
        {
            GE_SWITCH_TYPE(glm::vec3)
                changed = ImGui::ColorEdit3(label.c_str(), (float*) &vec, GE_EDITOR_COLOR_PICKER_FLAGS);
            else GE_SWITCH_TYPE(glm::vec4)
                changed = ImGui::ColorEdit4(label.c_str(), (float*) &vec, GE_EDITOR_COLOR_PICKER_FLAGS);
        #ifdef DEBUG
            else
                Log::Write("Color Picker used on type other than glm::vec3 or glm::vec4.\n");
        #endif
        }
        else
            switch(settings.ViewMode)
            {
            case ScalarViewMode::Slider:
                changed = ImGui::SliderScalarN(label.c_str(), type, &vec, COMPONENT_COUNT, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
                break;
            case ScalarViewMode::Input:
                changed = ImGui::InputScalarN(label.c_str(), type, &vec, COMPONENT_COUNT, &settings.Step, nullptr, nullptr, GE_EDITOR_INPUT_FLAGS);
                break;
            case ScalarViewMode::Drag:
            default:
                changed = ImGui::DragScalarN(label.c_str(), type, &vec, COMPONENT_COUNT, (float) settings.Step, &settings.Minimum, &settings.Maximum, nullptr, GE_EDITOR_INPUT_FLAGS);
            }

        /*if constexpr(!CONST)*/
        vec = glm::clamp(vec, settings.Minimum, settings.Maximum);

        if(!settings.Tooltip.empty() && ImGui::IsItemHovered(GE_EDITOR_TOOLTIP_FLAGS))
            ImGui::SetTooltip(settings.Tooltip.data());

        /*if constexpr (CONST)
            ImGui::EndDisabled();*/

        return changed;
    }

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>& settings, const glm::vec<COMPONENT_COUNT, T>& vec, u8 depth)
    {
        using RAW_T = glm::vec<COMPONENT_COUNT, T>;
        RAW_T temp = vec;

        ImGui::BeginDisabled(true);
            DrawField(settings, temp, depth);
        ImGui::EndDisabled();

        return false;
    }

    template <class T>
    bool DrawField(const ScalarField<T>& settings, glm::qua<T>& quat, u8 depth)
    {
        glm::vec<3, T> temp = glm::degrees(glm::eulerAngles(quat));
        const bool changed = DrawField(settings, temp, depth);
        quat = glm::radians(temp);

        return changed;
    }

    template <class T>
    bool DrawField(const ScalarField<T>& settings, const glm::qua<T>& quat, u8 depth)
    {
        glm::vec<4, T> temp = glm::degrees(glm::eulerAngles(quat));

        ImGui::BeginDisabled(true);
            DrawField(settings, temp, depth);
        ImGui::EndDisabled();

        return false;
    }

    template <class T, class SETTINGS_T>
    T* DrawField(const ArrayField<SETTINGS_T>& settings, Array<T>& ts, u8 depth)
    {
        return ts.begin() + DrawField(settings, ts.Data(), ts.Count(), depth);
    }

    template <class T, class SETTINGS_T>
    size_t DrawField(const ArrayField<SETTINGS_T>& settings, T* ts, size_t count, u8 depth)
    {
        const bool viewName = (bool)(settings.ViewMode & ArrayViewMode::Name);
        if((bool)(settings.ViewMode & ArrayViewMode::Stats))
        {
            ImGui::TextUnformatted(std::format("{}:\n\tType: {}\n\tSize: {}\n\tByte Count: {}\n\tPointer: {}",
                settings.Name,
                demangle(typeid(T).name()),
                count,
                sizeof(T) * count,
                (void*) ts
            ).c_str());
        }

        Field tempField = settings;
        tempField.Name = DEFAULT;

        if(viewName)
        {
            ImGui::TextUnformatted(settings.Name.data());
            ImGui::SameLine();
        }

        size_t changed = count;
        if((bool)(settings.ViewMode & ArrayViewMode::Elements) && ts)
        {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());
            ImGui::BeginTable("table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerV);
            ImGui::TableSetupColumn("Index");
            ImGui::TableSetupColumn(viewName ? "Value" : settings.Name.data());
            ImGui::TableHeadersRow();

            for(size_t i = 0; i < count; i++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(std::to_string(i).c_str());
                ImGui::TableSetColumnIndex(1);

                ImGui::PushID(i);
                if(DrawField(tempField, ts[i], depth + 1))
                {
                    GE_ASSERT(changed == count); // multiple fields happened to get updated in one go
                    changed = i;
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        return changed;
    }

    template<class T>
    bool DragDropCompare(ImGuiPayload& payload)
    {
        const bool isReflectable = payload.IsDataType(GE_EDITOR_ASSET_PAYLOAD);
        const Reference<Asset>& data = **(const Reference<Asset>**) payload.Data;

        if(!isReflectable) return false;
        return data->IsCastable<T>();
    }

    template <class T> requires std::is_base_of_v<Reflectable<Window*>, T>
    bool DrawField(const Field& settings, Reference<T>& ref, u8 depth)
    {
        using RAW_T = std::remove_const_t<T>;
        constexpr bool isConst = std::is_const_v<T>;

        ImGui::TextUnformatted(settings.Name.data());
        ImGui::SameLine();

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x + ImGui::GetScrollX());
        std::string_view type = "IReflectable (NO TYPE INFO)";
        if constexpr(requires(RAW_T e) { e.GetType(); })
            if(ref && ref->GetType())
                type = ref->GetType()->Name;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if(depth < 1) nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        const bool open = ImGui::TreeNodeEx(std::format("{} ({})##{}", type, (void*) ref.GetPointer(), settings.Name).c_str(), nodeFlags);

        bool changed = false;
        if(ImGui::BeginDragDropTarget())
        {
            if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropCompare<T>))
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