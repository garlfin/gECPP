//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>

#include "Serializable/Serializable.h"
#include "Utility/Macro.h"

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Resizable
#define GE_EDITOR_TOOLTIP_FLAGS ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled
#define GE_EDITOR_INPUT_FLAGS ImGuiSliderFlags_AlwaysClamp
#define GE_EDITOR_COLOR_PICKER_FLAGS ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf

namespace gE
{
#ifdef DEBUG
    template<class T, class OWNER_T>
    using GetterFunction = T (OWNER_T::*)() const;

    template<class T, class OWNER_T>
    using SetterFunction = void (OWNER_T::*)(T t);

    struct Field
    {
        std::string_view Name = DEFAULT;
        std::string_view Tooltip = DEFAULT;
    };

    enum class ScalarViewMode : u8
    {
        Input,
        Drag,
        Slider,
        ColorPicker // ONLY COMPATIBLE WITH VEC3 AND VEC4
    };

    template<class T>
    struct ScalarField : public Field
    {
        T Minimum = std::numeric_limits<T>::lowest();
        T Maximum = std::numeric_limits<T>::max();
        T Step = T(1);
        ScalarViewMode ViewMode = ScalarViewMode::Drag;
    };

    template<>
    struct ScalarField<float> : public Field
    {
        float Minimum = std::numeric_limits<float>::lowest();
        float Maximum = std::numeric_limits<float>::max();
        float Step = 0.01f;
        ScalarViewMode ViewMode = ScalarViewMode::Drag;
    };

    template<>
    struct ScalarField<double> : public Field
    {
        double Minimum = std::numeric_limits<double>::lowest();
        double Maximum = std::numeric_limits<double>::max();
        float Step = 0.01;
        ScalarViewMode ViewMode = ScalarViewMode::Drag;
    };

    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET_VALUE(bool, ConsoleOpen, _isConsoleOpen);
        GET_SET_VALUE(Entity*, SelectedEntity, _activeEntity);
        GET_SET_VALUE(bool, IsRunning, _running);

        template<class T, class SETTINGS_T>
        static bool DrawField(const SETTINGS_T&, T&, u8 depth);

        template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
        static bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<OUT_T, OWNER_T>, SetterFunction<IN_T, OWNER_T>);

    private:
        void DrawLog();

        Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        bool _isConsoleOpen = false;
        bool _running = true;
        size_t _oldLogSize = 0;
    };
#endif
}

#include "Editor.inl"
