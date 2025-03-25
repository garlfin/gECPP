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

	struct AccessorData
	{
		const gltf::Accessor* Accessor = nullptr;
		const gltf::BufferView* View = nullptr;
		const gltf::Buffer* Buffer = nullptr;
	};

	struct PrimitiveData
	{
		const gltf::Attribute* Attribute = nullptr;
		AccessorData Accessor = DEFAULT;

		ALWAYS_INLINE operator bool() const { return Accessor.Accessor; }
		ALWAYS_INLINE AccessorData* operator->() { return &Accessor; }
		ALWAYS_INLINE const AccessorData* operator->() const { return &Accessor; }
	};

	struct ChannelData
	{
		const gltf::AnimationChannel* Channel = nullptr;
		const gltf::AnimationSampler* Sampler = nullptr;
		AccessorData Input = DEFAULT;
		AccessorData Output = DEFAULT;
	};

	struct Vertex
	{
		glm::vec3 Position;
		alignas(8) glm::vec2 UV;
		alignas(4) glm::i8vec3 Normal;
		alignas(4) glm::i8vec4 Tangent;
	};

	struct VertexWeight
	{
		glm::u8vec4 Bones;
		glm::u8vec4 Weights;

		inline u8 AddWeight(u16 bone, u8 weight);
	};

	struct Face
	{
		glm::u32vec3 Triangle;
	};

    template<class FIELD_T, class STRUCT>
    NODISCARD GPU::VertexField CreateField(FIELD_T STRUCT::* dst, const char[4], u8 index, u8 bufIndex);

	template<class FROM_T>
	FROM_T AccessBuffer(const AccessorData& accessor, size_t index, size_t stride = 0);

	template<class FROM_T, class TO_T, class SPAN_T>
	void FillBuffer(const GPU::VertexField& field, std::span<SPAN_T> dstSpan, const PrimitiveData& primitive, ConversionFunc<TO_T, FROM_T> func);

	NODISCARD inline glm::i8vec3 ConvertNormal(const glm::vec3& normal) noexcept;
	NODISCARD inline glm::i8vec4 ConvertTangent(const glm::vec4& normal) noexcept;
	NODISCARD inline glm::u8vec4 ConvertWeight(const glm::vec4& weight) noexcept;

	AccessorData GetAccessorData(const gltf::Asset& file, size_t index);
	PrimitiveData GetAttributeData(const gltf::Asset& file, const gltf::Primitive& prim, std::string_view attribute);
	PrimitiveData GetIndicesData(const gltf::Asset& file, const gltf::Primitive& prim);
	ChannelData GetChannelData(const gltf::Asset& file, const gltf::Animation& animation, const gltf::AnimationChannel& channel);
}

#include "MeshUtility.inl"

namespace gE::Model
{
	GLOBAL GPU::VertexField POSITION_FIELD = CreateField(&Vertex::Position, "POS", 0, 0);
	GLOBAL GPU::VertexField UV_FIELD = CreateField(&Vertex::UV, "UV0", 1, 0);
	GLOBAL GPU::VertexField NORMAL_FIELD = CreateField(&Vertex::Normal, "NOR", 2, 0);
	GLOBAL GPU::VertexField TANGENT_FIELD = CreateField(&Vertex::Tangent, "TAN", 3, 0);

	GLOBAL GPU::VertexField BONES_FIELD = CreateField(&VertexWeight::Bones, "BONE", 3, 1);
	GLOBAL GPU::VertexField WEIGHTS_FIELD = CreateField(&VertexWeight::Weights, "WGHT", 3, 1);

	GLOBAL GPU::VertexField INDICES_FIELD = CreateField(&Face::Triangle, "TRI", 0, 1);
}
