//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "ShaderStageType.h"
#include "Preprocessor.h"

namespace GPU
{
	struct ShaderStage : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(ShaderStage, Serializable);
		DEFAULT_CM_CONSTRUCTOR(ShaderStage);

	public:
		ShaderStageType Type;
		std::string Source;

		ALWAYS_INLINE void Free() override { Source.clear(); Source.shrink_to_fit(); };
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Source.data(); }
	};

	struct Shader : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(Shader, Serializable);
		DEFAULT_CM_CONSTRUCTOR(Shader);

	public:
		ShaderStage VertexStage;
		ShaderStage FragmentStage;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	struct ComputeShader : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(ComputeShader, Serializable);
		DEFAULT_CM_CONSTRUCTOR(ComputeShader);

	public:
		ShaderStage ComputeStage;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
}

#include <Graphics/API/GL/Shader/Shader.h>
