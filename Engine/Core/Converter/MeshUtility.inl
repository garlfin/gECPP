//
// Created by scion on 3/6/2025.
//

#pragma once

#include "MeshUtility.h"

namespace gE::Model
{
    template<class T, class S>
    NODISCARD GPU::VertexField CreateField(T S::* dst, const char name[4], u8 index, u8 bufIndex) // Grr me want make constexpr
    {
        GPU::VertexField field;

        std::copy_n(name, sizeof(field.Name), field.Name);
        field.ElementType = (GPU::ElementType) GLType<typename T::value_type>;
        field.ElementCount = T::length();
        field.Index = index;

        field.BufferIndex = bufIndex;
        field.Offset = (u8) (size_t) &((S*) nullptr->*dst); // Work of the Devil?
        field.Normalized = !std::is_floating_point_v<typename T::value_type>;

        return field;
    }

    template <class FROM_T>
    FROM_T AccessBuffer(const AccessorData& accessor, size_t index, size_t stride)
    {
        if(!stride)
            stride = accessor.View->byteStride.value_or(gltf::getElementByteSize(accessor.Accessor->type, accessor.Accessor->componentType));

        const gltf::sources::Array* data = std::get_if<gltf::sources::Array>(&accessor.Buffer->data);
        GE_ASSERT(data);

        return *(const FROM_T*) &data->bytes[accessor.View->byteOffset + accessor.Accessor->byteOffset + stride * index];
    }

    template<class FROM_T, class TO_T, class SPAN_T>
    void FillBuffer(const GPU::VertexField& field, std::span<SPAN_T> dstSpan, const PrimitiveData& primitive, ConversionFunc<TO_T, FROM_T> func)
    {
        const gltf::Accessor& accessor = *primitive->Accessor;
        const gltf::BufferView& bufView = *primitive->View;

        GE_ASSERT(GLType<typename FROM_T::value_type> == gltf::getGLComponentType(accessor.componentType));
        GE_ASSERT(FROM_T::length() == gltf::getNumComponents(accessor.type));
        // GE_ASSERT(field.Normalized == accessor.normalized);

        const gltf::sources::Array* data = std::get_if<gltf::sources::Array>(&primitive->Buffer->data);
        GE_ASSERT(data);

        for(size_t i = 0; i < accessor.count; i++)
        {
            TO_T* dst = (TO_T*) ((std::byte*) &dstSpan[i] + field.Offset);

            if constexpr (std::is_same_v<FROM_T, TO_T>)
                *dst = AccessBuffer<FROM_T>(primitive.Accessor, i);
            else
                *dst = func(AccessBuffer<FROM_T>(primitive.Accessor, i));
        }
    }

    inline u8 VertexWeight::AddWeight(u16 bone, u8 weight)
    {
        for(u8 i = 0; i < 4; i++)
        {
            if(Weights[i]) continue;

            Bones[i] = bone;
            Weights[i] = weight;
            return i;
        }

        return -1;
    }

    inline glm::i8vec3 ConvertNormal(const glm::vec3& normal) noexcept
    {
        const glm::vec3 vec = clamp(normal, glm::vec3(-1.f), glm::vec3(1.f));
        return vec * glm::vec3(INT8_MAX);
    }

    inline glm::i8vec4 ConvertTangent(const glm::vec4& normal) noexcept
    {
        glm::vec4 vec = clamp(normal, glm::vec4(-1.f), glm::vec4(1.f));

#ifdef GE_MODEL_FLIP_HANDEDNESS
        vec.w *= -1.f;
#endif

        return vec * glm::vec4(INT8_MAX);
    }

    inline glm::u8vec4 ConvertWeight(const glm::vec4& weight) noexcept
    {
        const glm::vec4 vec = clamp(weight, glm::vec4(0.f), glm::vec4(1.f));
        return glm::u8vec4(vec * glm::vec4(UINT8_MAX));
    }
}
