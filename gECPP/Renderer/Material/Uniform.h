//
// Created by scion on 1/17/2025.
//

#pragma once

#include "Shader.h"

namespace gE
{
    class DynamicUniform
    {
    public:
        explicit DynamicUniform(const Shader&, u32 location);
        explicit DynamicUniform(const Shader&, const std::string& name);

        GET_CONST(const Shader&, Shader, *_shader);
        GET_CONST(u32, , _location);

        template<class T>
        ALWAYS_INLINE void Set(const T& t) const
        {
            if(_location != -1u) _shader->GetShader().SetUniform(_location, t);
        }

    private:
        const Shader* _shader;
        u32 _location;
    };

    template<>
    void DynamicUniform::Set(const API::Texture& t) const;

    template<class T>
    class Uniform : private DynamicUniform
    {
    public:
        using DynamicUniform::DynamicUniform;

        using DynamicUniform::Get;
        using DynamicUniform::GetShader;

        ALWAYS_INLINE void Set(const T& t) const { DynamicUniform::Set(t); }
    };

    template<class T>
    class ValueUniform : private Uniform<T>
    {
    public:
        ValueUniform(const Shader& mat, const char* n, const T& t) : Uniform<T>(mat, n), _t(t) { };
        ValueUniform(const Shader& mat, const char* n, T&& t) : Uniform<T>(mat, n), _t(t) { };
        ValueUniform(const Shader& mat, u32 l, const T& t) : Uniform<T>(mat, l), _t(t) { };
        ValueUniform(const Shader& mat, u32 l, T&& t) : Uniform<T>(mat, l), _t(t) { };

        ValueUniform(const ValueUniform&) = default;
        ValueUniform(ValueUniform&&) = default;

        ALWAYS_INLINE ValueUniform& operator=(const T& t) { _t = t; return *this; }
        ALWAYS_INLINE ValueUniform& operator=(T&& t) { _t = t; return *this; }

        ALWAYS_INLINE ValueUniform& operator=(ValueUniform&&) = default;
        ALWAYS_INLINE ValueUniform& operator=(const ValueUniform&) = default;
        ALWAYS_INLINE T* operator->() const { return _t; }
        ALWAYS_INLINE T& operator*() const { return *_t; }
        ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
        ALWAYS_INLINE operator T*() const { return _t; } // NOLINT
        ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

        ALWAYS_INLINE void Set() const { Uniform<T>::Set(_t); }

        GET_SET(T&, , _t);
        SET_XVAL(T, , _t);

    private:
        T _t;
    };

    template<class T>
    using ReferenceUniform = ValueUniform<Reference<T>>;

    template<class T>
    using SmartPointerUniform = ValueUniform<Pointer<T>>;
}