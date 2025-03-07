//
// Created by scion on 3/1/2025.
//

#pragma once

#include "MeshLoader.h"

#include <Vendor/FastGLTF/core.hpp>

namespace gltf = fastgltf;

#define GE_MODEL_FLIP_HANDEDNESS

namespace gE::Model
{
	template<class TO_T, class FROM_T>
	using ConversionFunc = TO_T(*)(const FROM_T& from) noexcept;

	struct PrimitiveData
	{
		const gltf::Attribute* Attribute = nullptr;
		const gltf::Accessor* Accessor = nullptr;
		const gltf::BufferView* BufferView = nullptr;
		const gltf::Buffer* Buffer = nullptr;

		ALWAYS_INLINE operator bool() const { return Accessor; }
	};

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 UV;
		glm::i8vec3 Normal;
		alignas(4) glm::i8vec4 Tangent;
	};

	struct Face
	{
		glm::u32vec3 Triangle;
	};

    template<class FIELD_T, class STRUCT>
    NODISCARD GPU::VertexField CreateField(FIELD_T STRUCT::* dst, const char[4], u8 index, u8 bufIndex);

	template<class FROM_T, class TO_T, class SPAN_T>
	void FillBuffer(const GPU::VertexField& field, std::span<SPAN_T> dstSpan, const PrimitiveData& primitive, ConversionFunc<TO_T, FROM_T> func);

	NODISCARD inline glm::i8vec3 ConvertNormal(const glm::vec3& normal) noexcept;
	NODISCARD inline glm::i8vec4 ConvertTangent(const glm::vec4& normal) noexcept;
}

#include "MeshUtility.inl"

namespace gE::Model
{
	GLOBAL GPU::VertexField POSITION_FIELD = CreateField(&Vertex::Position, "POS", 0, 0);
	GLOBAL GPU::VertexField UV_FIELD = CreateField(&Vertex::UV, "UV0", 1, 0);
	GLOBAL GPU::VertexField NORMAL_FIELD = CreateField(&Vertex::Normal, "NOR", 2, 0);
	GLOBAL GPU::VertexField TANGENT_FIELD = CreateField(&Vertex::Tangent, "TAN", 3, 0);

	GLOBAL GPU::VertexField INDICES_FIELD = CreateField(&Face::Triangle, "TRI", 0, 1);
}
