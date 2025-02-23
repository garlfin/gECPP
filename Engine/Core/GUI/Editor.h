//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>
#include <Core/Serializable/Serializable.h>
#include <IMGUI/imgui.h>

#include "Core/AssetManager.h"

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Resizable
#define GE_EDITOR_TOOLTIP_FLAGS ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled
#define GE_EDITOR_INPUT_FLAGS ImGuiSliderFlags_AlwaysClamp
#define GE_EDITOR_COLOR_PICKER_FLAGS ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf
#define GE_EDITOR_ICON_PADDING 16

CONSTEXPR_GLOBAL const char* GE_EDITOR_ASSET_PAYLOAD = "ASSET";

namespace gE
{
#ifdef GE_ENABLE_EDITOR
    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET_VALUE(bool, EditorOpen, _isEditorOpen);
        GET_SET_VALUE(Entity*, SelectedEntity, _activeEntity);
        GET_SET_VALUE(bool, IsRunning, _running);

        GET_SET(Path&, AssetPath, _assetPath);
        SET_XVAL(Path, AssetPath, _assetPath);

    private:
        static void DrawEntityDrawer();
        void DrawLog();
        void DrawInspector();
        void DrawHierarchy();
        void DrawAssetManager();

        Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        bool _isEditorOpen = false;
        bool _running = true;
        size_t _oldLogSize = 0;
        Path _assetPath = "";
        u16 _assetScale = 64;
    };
#endif

#ifdef GE_ENABLE_IMGUI
    template<class T, class OWNER_T>
    using GetterFunction = T (OWNER_T::*)() const;

    template<class T, class OWNER_T>
    using SetterFunction = void (OWNER_T::*)(T t);

    struct Field
    {
        std::string_view Name = DEFAULT;
        std::string_view Tooltip = DEFAULT;
    };

    enum class ArrayViewMode : u8
    {
        Stats = 1,
        Elements = 1 << 1
    };

    ENUM_OPERATOR(ArrayViewMode, &);
    ENUM_OPERATOR(ArrayViewMode, |);

    template<class T>
    struct ArrayField : public T
    {
        ArrayViewMode ViewMode = ArrayViewMode::Stats | ArrayViewMode::Elements;
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

    template<class T>
    CONSTEXPR_GLOBAL ImGuiDataType IMType = 0;

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T*, u8 depth);

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, glm::vec<COMPONENT_COUNT, T>&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const ArrayField<SETTINGS_T>&, Array<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const ArrayField<SETTINGS_T>&, T*, size_t, u8 depth);

    template<class T> requires std::is_base_of_v<Reflectable<Window*>, T>
    bool DrawField(const Field& settings, Reference<T>&, u8 depth);

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<OUT_T, OWNER_T>, SetterFunction<IN_T, OWNER_T>);
#endif
}

#include "Editor.inl"