//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>
#include <Serializable/Serializable.h>

#include "ShaderStageType.h"

namespace GL
{
	class ShaderStage;
	class Shader;
	class ComputeShader;
}

namespace GPU
{
	struct ShaderStage : public gE::Asset
	{
		SERIALIZABLE_PROTO("STGE", 1, ShaderStage, Asset);
		SERIALIZABLE_REFLECTABLE(ShaderStage, "GPU::ShaderStage");

	public:
		ShaderStage(ShaderStageType, const Path&);

		ShaderStageType StageType = DEFAULT;
		std::string Source = DEFAULT;
		Path BasePath = DEFAULT;

		ALWAYS_INLINE void Free() override { Source.clear(); };
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Source.empty(); }

		~ShaderStage() override
		{
			ASSET_CHECK_FREE(ShaderStage);
		}
	};

	struct Shader : public gE::Asset
	{
		SERIALIZABLE_PROTO("SHDR", 1, Shader, Asset);
		SERIALIZABLE_REFLECTABLE(Shader, "GPU::Shader");

	public:
		Shader(const Path& v, const Path& f);

		ShaderStage VertexStage = DEFAULT;
		ShaderStage FragmentStage = DEFAULT;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	struct ComputeShader : public gE::Asset
	{
		SERIALIZABLE_PROTO("COMP", 1, ComputeShader, Asset);
		SERIALIZABLE_REFLECTABLE(ComputeShader, "GPU::ComputeShader");

	public:
		explicit ComputeShader(const Path& c);

		ShaderStage ComputeStage = DEFAULT;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
}

#if API == GL
	#include <Graphics/API/GL/Shader/Shader.h>
#endif
