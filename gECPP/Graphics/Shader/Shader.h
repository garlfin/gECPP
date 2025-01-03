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
	struct ShaderStage : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(STGE, 1, ShaderStage, Serializable);
		API_REFLECTABLE(ShaderStage, "GPU::ShaderStage", API::ShaderStage);

	public:
		ShaderStage(ShaderStageType, const Path&);
		DEFAULT_OPERATOR_CM(ShaderStage);

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

	struct Shader : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(SHDR, 1, Shader, Serializable);
		API_REFLECTABLE(Shader, "GPU::Shader", API::Shader);
		DEFAULT_OPERATOR_COPY(Shader);

	public:
		Shader(const Path& v, const Path& f);

		// implicit constructor was giving me issues...
		OPERATOR_MOVE_IMPL(Shader, Shader::Free(),,
			FragmentStage = move(o.FragmentStage);
			VertexStage = move(o.VertexStage);
		);

		ShaderStage VertexStage = DEFAULT;
		ShaderStage FragmentStage = DEFAULT;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	struct ComputeShader : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(COMP, 1, ComputeShader, Serializable);
		API_REFLECTABLE(ComputeShader, "GPU::ComputeShader", API::ComputeShader);

	public:
		explicit ComputeShader(const Path& c);
		DEFAULT_OPERATOR_CM(ComputeShader);

		ShaderStage ComputeStage = DEFAULT;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
}

#if API == GL
	#include <Graphics/API/GL/Shader/Shader.h>
#endif
