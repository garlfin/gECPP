//
// Created by scion on 6/30/2025.
//

#pragma once

#include <type_traits>
#include <Core/Math/Math.h>

template<class SERIALIZABLE_T>
concept ReflConstructible = requires
{
    SERIALIZABLE_T::SType;
};

template<class T>
concept HasType = requires(std::add_const_t<T>& t)
{
    t.GetType();
};

template<class T>
concept HasOnGUI = requires(T& t, u8 depth)
{
    t.OnEditorGUI(depth);
};

template<class T>
concept HasEditorName = requires(std::add_const_t<T>& t)
{
    { t.GetEditorName() } -> std::convertible_to<std::string>;
};