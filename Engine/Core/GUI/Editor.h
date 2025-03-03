//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>
#include <Core/Pointer.h>
#include <Core/Serializable/Asset.h>
#include <Core/Serializable/Serializable.h>
#include <IMGUI/imgui.h>
#include <SDL3/SDL_dialog.h>

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Resizable
#define GE_EDITOR_TOOLTIP_FLAGS ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled
#define GE_EDITOR_INPUT_FLAGS ImGuiSliderFlags_AlwaysClamp
#define GE_EDITOR_COLOR_PICKER_FLAGS ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf
#define GE_EDITOR_ICON_PADDING 16

#define GE_EDITOR_FILTER_TEXTURE 0
#define GE_EDITOR_FILTER_MODEL 1
#define GE_EDITOR_FILTER_FILE 2

CONSTEXPR_GLOBAL const char* GE_EDITOR_ASSET_PAYLOAD = "ASSET";

namespace gE
{
#ifdef GE_ENABLE_EDITOR
    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET(bool, EditorOpen, _isEditorOpen);
        GET_SET(Entity*, SelectedEntity, _activeEntity);
        GET_SET(bool, IsRunning, _running);

        GET_SET(Path&, AssetPath, _assetPath);
        SET_XVAL(Path, AssetPath, _assetPath);

    private:
        static void DrawEntityDrawer();
        void DrawLog();
        void DrawInspector();
        void DrawHierarchy();
        void DrawAssetManager();

        static void LoadFileCallback(Editor* editor, const char* const* paths, int filter);
        static void ImportFileCallback(Editor* editor, const char* const* paths, int filter);

        constexpr static std::array<SDL_DialogFileFilter, 2> Filters
        {
            SDL_DialogFileFilter("Textures: .pvr", "pvr"),
            SDL_DialogFileFilter("3D Models: .dae, .fbx, .obj, .gltf, .glb", "dae;fbx;obj;gltf;glb")
        };

        Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        const File* _activeAsset = DEFAULT;
        Path _assetPath = "";

        std::mutex _loadingMutex = DEFAULT;
        Path _loadingPath = DEFAULT;
        u8 _loadingFilter = -1;
        const Type<Window*>* _loadingType = nullptr;

        bool _isEditorOpen = false;
        bool _running = true;
        size_t _oldLogSize = 0;
        u16 _iconSize = 128;
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
        Elements = 1 << 1,
        Name = 1 << 2
    };

    ENUM_OPERATOR(ArrayViewMode, &);
    ENUM_OPERATOR(ArrayViewMode, |);

    template<class T>
    struct ArrayField : public T
    {
        ArrayViewMode ViewMode = ArrayViewMode::Elements;
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
        T Step = std::is_floating_point_v<T> ? T(0.01) : T(1);
        ScalarViewMode ViewMode = ScalarViewMode::Drag;
    };

    template<class T, size_t SIZE>
    struct EnumField : public Field
    {
        const EnumData<T, SIZE>& Type;
    };

    template<class T>
    CONSTEXPR_GLOBAL ImGuiDataType IMType = 0;

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T*, u8 depth);

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, glm::vec<COMPONENT_COUNT, T>&, u8 depth);

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, const glm::vec<COMPONENT_COUNT, T>&, u8 depth);

    template<class T, class SETTINGS_T>
    T* DrawField(const ArrayField<SETTINGS_T>&, Array<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    size_t DrawField(const ArrayField<SETTINGS_T>&, T*, size_t, u8 depth);

    template<class T> requires std::is_base_of_v<Reflectable<Window*>, T>
    bool DrawField(const Field& settings, Reference<T>&, u8 depth);

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<OUT_T, OWNER_T>, SetterFunction<IN_T, OWNER_T>);
#endif
}

#include "Editor.inl"