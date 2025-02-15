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
	struct ShaderSource final : public gE::Asset
	{
		SERIALIZABLE_PROTO("SSRC", 0, ShaderSource, Asset);
		REFLECTABLE_PROTO(ShaderSource, gE::Asset, "GPU::ShaderSource") {};

	public:
		explicit ShaderSource(const Path&);
		explicit ShaderSource(const std::string& src) : Source(src) {};
		explicit ShaderSource(std::string&& src) : Source(std::move(src)) {};

		void Free() override { Source.clear(); Source.shrink_to_fit(); }
		NODISCARD bool IsFree() const override { return Source.empty(); }

		explicit operator const std::string&() const { return Source; }

		std::string Source;
	};

	struct ShaderStage : public gE::Asset
	{
		SERIALIZABLE_PROTO("STGE", 1, ShaderStage, Asset);
		REFLECTABLE_PROTO(ShaderStage, gE::Asset, "GPU::ShaderStage") {};

	public:
		ShaderStage(ShaderStageType, const Path&);

		ShaderStageType StageType = DEFAULT;
		ShaderSource Source;
		Path BasePath = DEFAULT;

		ALWAYS_INLINE void Free() override { Source.Free(); };
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Source.IsFree(); }

		~ShaderStage() override
		{
			ASSET_CHECK_FREE(ShaderStage);
		}
	};

	struct Shader : public gE::Asset
	{
		SERIALIZABLE_PROTO("SHDR", 1, Shader, Asset);
		REFLECTABLE_PROTO(Shader, gE::Asset, "GPU::Shader") {};

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
		REFLECTABLE_PROTO(ComputeShader, gE::Asset, "GPU::ComputeShader") {};

	public:
		explicit ComputeShader(const Path& c);

		ShaderStage ComputeStage = DEFAULT;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
	inline REFLECTABLE_FACTORY_IMPL(ShaderSource, ShaderSource);
}

#if API == GL
	#include <Graphics/API/GL/Shader/Shader.h>
#endif
