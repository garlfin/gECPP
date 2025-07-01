//
// Created by scion on 6/30/2025.
//

#pragma once

#include "IO.h"

#include <Core/Binary.h>
#include <Core/Pointer.h>
#include <Core/GUI/Editor/Settings.h>

struct IReflectable;

namespace gE
{
    class Window;
}

enum class FieldFlags : u8
{
    Serialize = 1,
    Display = 1 << 1,
    ReadOnly = 1 << 2,
    Default = Serialize | Display
};

ENUM_OPERATOR(FieldFlags, |);
ENUM_OPERATOR(FieldFlags, &);

namespace gE::Editor
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
        Name = 1 << 2,
        Default = Elements
    };

    ENUM_OPERATOR(ArrayViewMode, &);
    ENUM_OPERATOR(ArrayViewMode, |);

    template<class T>
    struct ArrayField : public T
    {
        ArrayViewMode ViewMode = ArrayViewMode::Default;
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
    CONSTEXPR_GLOBAL ImGuiDataType IMType = 0;

    template<class T>
    std::string GetLabel(std::add_const_t<T>* t, std::string_view defaultType = DEFAULT);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T&, u8 depth);

    template<class T, class SETTINGS_T>
    bool DrawField(const SETTINGS_T&, T*, u8 depth);

    template <class T, length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, vec<COMPONENT_COUNT, T>&, u8 depth);

    template <class T, length_t COMPONENT_COUNT>
    bool DrawField(const ScalarField<T>&, const vec<COMPONENT_COUNT, T>&, u8 depth);

    template <class T>
    bool DrawField(const ScalarField<T>&, qua<T>&, u8 depth);

    template <class T>
    bool DrawField(const ScalarField<T>&, const qua<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    T* DrawField(const ArrayField<SETTINGS_T>&, Array<T>&, u8 depth);

    template<class T, class SETTINGS_T>
    size_t DrawField(const ArrayField<SETTINGS_T>&, T*, size_t, u8 depth);

    template<class SETTINGS_T, class OWNER_T, class OUT_T, class IN_T>
    bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<OUT_T, OWNER_T>, SetterFunction<IN_T, OWNER_T>);

    template<class BASE_T, class T = BASE_T, class SETTINGS_T = std::nullptr_t> requires IsDragDroppable<BASE_T, T>
    bool DrawField(const DragDropField<BASE_T, T, SETTINGS_T>& settings, Reference<T>&, u8 depth);
}

class IField
{
public:
    using MemberPtr = std::byte IReflectable::*;

    GET_CONST(const std::string&, Name, _name);
    GET_CONST(const std::string&, Tooltip, _tooltip);
    GET_CONST(MemberPtr, MemberPointer, _member);
    GET_CONST(FieldFlags, Flags, _flags);

    virtual void Serialize(IReflectable&, std::istream& stream, gE::Window* window) const = 0;
    virtual void Draw(IReflectable&, u8 guiDepth) const = 0;

    virtual ~IField() = default;

protected:
    IField(gE::Editor::Field&& field, MemberPtr member, FieldFlags flags) :
        _name(std::move(field.Name)),
        _tooltip(std::move(field.Tooltip)),
        _member(member),
        _flags(flags)
    {}

    template<class SETTINGS_T, class FIELD_T>
    void DrawFieldBase(const SETTINGS_T& field, FIELD_T& t, u8 guiDepth) const
    {
        if((bool) (_flags & FieldFlags::ReadOnly))
            gE::Editor::DrawField(field, (const FIELD_T&) t, guiDepth);
        else
            gE::Editor::DrawField(field, t, guiDepth);
    }

    template<class T>
    T& GetMember(IReflectable& obj) const
    {
        return obj.*((T IReflectable::*) _member);
    }

    gE::Editor::Field GetFieldSettings() const { return gE::Editor::Field{ _name, _tooltip }; }

private:
    std::string _name;
    std::string _tooltip;
    MemberPtr _member = nullptr;
    FieldFlags _flags = DEFAULT;
};

template<class T> requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
class Field : public IField
{
public:
    using MemberPtr = T IReflectable::*;

    template<class I> requires std::is_base_of_v<IReflectable, I>
    Field(T I::* member, gE::Editor::Field&& settings, FieldFlags flags) :
        IField(std::move(settings), (IField::MemberPtr) member, flags)
    {}

    GET_CONST(MemberPtr, MemberPointer, (MemberPtr) IField::GetMemberPointer());

    void Serialize(IReflectable& obj, std::istream& stream, gE::Window* window) const override
    {
        Read(stream, GetMember<T>(obj));
    }

    void Draw(IReflectable& obj, u8 guiDepth) const override
    {
        DrawFieldBase(GetFieldSettings(), GetMember<T>(obj), guiDepth);
    }
};

template<class T> requires std::is_scalar_v<T>
class ScalarField : public IField
{
public:
    using MemberPtr = T IReflectable::*;

    template<class I> requires std::is_base_of_v<IReflectable, I>
    ScalarField(T I::* member, gE::Editor::ScalarField<T>&& settings, FieldFlags flags) : IField(std::move(settings), (IField::MemberPtr) member, flags),
        _minimum(settings.Minimum),
        _maximum(settings.Maximum),
        _step(settings.Step),
        _viewMode(settings.ViewMode)
    {}

    GET_CONST(MemberPtr, MemberPointer, (MemberPtr) IField::GetMemberPointer());

    void Serialize(IReflectable& obj, std::istream& stream, gE::Window* window) const override
    {
        Read<T>(stream, GetMember<T>(obj));
    }

    void Draw(IReflectable& obj, u8 guiDepth) const override
    {
        gE::Editor::ScalarField field { GetName(), GetTooltip(), _minimum, _maximum, _step };
        DrawFieldBase(GetFieldSettings(), GetMember<T>(obj), guiDepth);
    }

protected:
    gE::Editor::ScalarField<T> GetFieldSettings() const
    {
        return gE::Editor::ScalarField<T>{ IField::GetFieldSettings(), _minimum, _maximum, _step };
    }

private:
    T _minimum;
    T _maximum;
    T _step;
    gE::Editor::ScalarViewMode _viewMode;
};

template<class T, length_t COMPONENT_COUNT>
class VectorField : public ScalarField<T>
{
public:
    using VEC_T = vec<COMPONENT_COUNT, T>;
    using MemberPtr = VEC_T IReflectable::*;

    template<class I> requires std::is_base_of_v<IReflectable, I>
    VectorField(VEC_T I::* member, gE::Editor::ScalarField<T>&& settings, FieldFlags flags) :
        ScalarField<T>(std::move(settings), member, flags)
    {}

    GET_CONST(MemberPtr, MemberPointer, (MemberPtr) GetMemberPointer());

    void Serialize(IReflectable& obj, std::istream& stream, gE::Window* window) const override
    {
        Read<VEC_T>(stream, IField::GetMember<VEC_T>(obj));
    }

    void Draw(IReflectable& obj, u8 guiDepth) const override
    {
        IField::DrawFieldBase(ScalarField<T>::GetFieldSettings(), IField::GetMember<VEC_T>(obj), guiDepth);
    }
};

#include "Field.inl"