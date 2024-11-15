//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>
#include "ShaderStageType.h"

namespace GPU
{
	struct ShaderStage : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(STGE, 1, ShaderStage, Serializable);

	public:
		DEFAULT_OPERATOR_CM(ShaderStage);

		ShaderStageType Type;
		std::string Source;

		ALWAYS_INLINE void Free() override { Source.clear(); Source.shrink_to_fit(); };
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Source.data(); }

		~ShaderStage() override { ASSET_CHECK_FREE(ShaderStage); }
	};

	struct Shader : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(SHDR, 1, Shader, Serializable);

	public:
		DEFAULT_OPERATOR_CM(Shader);

		ShaderStage VertexStage;
		ShaderStage FragmentStage;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	struct ComputeShader : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(COMP, 1, ComputeShader, Serializable);

	public:
		DEFAULT_OPERATOR_CM(ComputeShader);

		ShaderStage ComputeStage;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
}

#include <Graphics/API/GL/Shader/Shader.h>
