//
// Created by scion on 3/3/2025.
//

#pragma once

#include <string>
#include <Core/Macro.h>
#include <Vendor/IMGUI/imgui.h>

#include "Editor/Settings.h"

#ifdef GE_ENABLE_IMGUI
namespace gE
{
    template<class T, class OWNER_T>
    using GetterFunction = T (OWNER_T::*)() const;

    template<class T, class OWNER_T>
    using SetterFunction = void (OWNER_T::*)(T t);

    template<class BASE_T, class T>
    concept IsDragDroppable = std::is_base_of_v<BASE_T, T> || std::is_same_v<BASE_T, T>;

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

    using NoUserData = const std::nullptr_t*;

    template<class BASE_T, class SETTINGS_T = std::nullptr_t>
    using DragDropCompareFunc = bool(*)(const Reference<BASE_T>&, const SETTINGS_T* userData);

    template<class BASE_T, class T, class SETTINGS_T = std::nullptr_t> requires IsDragDroppable<BASE_T, T>
    struct DragDropField : public Field
    {
        std::string Type = "";
        DragDropCompareFunc<BASE_T> Acceptor = nullptr;
        const SETTINGS_T* UserData = DEFAULT;
    };

    template<class T>
    bool AssetDragDropAcceptor(const Reference<Asset>& asset, NoUserData)
    {
        return asset->IsCastable<T>();
    }

    template<class T, class SETTINGS_T> requires IsDragDroppable<Asset, T>
    struct DragDropField<Asset, T, SETTINGS_T> : public Field
    {
        std::string Type = GE_EDITOR_ASSET_PAYLOAD;
        DragDropCompareFunc<Asset, SETTINGS_T> Acceptor = AssetDragDropAcceptor<T>;
        SETTINGS_T* UserData = DEFAULT;
    };

    template<class T, class SETTINGS_T = std::nullptr_t>
    using AssetDragDropField = DragDropField<Asset, T, SETTINGS_T>;

    template<class T>
    CONSTEXPR_GLOBAL ImGuiDataType IMType = 0;

    template<class T>
    std::string GetLabel(std::add_const_t<T>* t, std::string_view defaultType = DEFAULT);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T*, u8 depth);

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, glm::vec<COMPONENT_COUNT, T>&, u8 depth);

    template <class T, glm::length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, const glm::vec<COMPONENT_COUNT, T>&, u8 depth);

    template <class T>
    bool DrawField(const ScalarField<T>&, glm::qua<T>&, u8 depth);

    template <class T>
    bool DrawField(const ScalarField<T>&, const glm::qua<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    T* DrawField(const ArrayField<SETTINGS_T>&, Array<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    size_t DrawField(const ArrayField<SETTINGS_T>&, T*, size_t, u8 depth);

    template<class BASE_T, class T = BASE_T, class SETTINGS_T = std::nullptr_t> requires IsDragDroppable<BASE_T, T>
    bool DrawField(const DragDropField<BASE_T, T, SETTINGS_T>& settings, Reference<T>&, u8 depth);

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<OUT_T, OWNER_T>, SetterFunction<IN_T, OWNER_T>);
}
#endif

#include "Field.inl"
