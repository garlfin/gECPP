//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Core/Serializable/Asset.h>
#include <Core/Serializable/Serializable.h>
#include <Graphics/Graphics.h>

#include "ShaderStageType.h"

#define GE_SS_MIN_VIEW_HEIGHT 512

namespace GPU
{
	struct ShaderSource final : public gE::Asset
	{
		SERIALIZABLE_PROTO(ShaderSource, Asset);

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
		SERIALIZABLE_PROTO(ShaderStage, Asset);

	public:
		ShaderStage(ShaderStageType, const Path&);

		ShaderStageType StageType = DEFAULT;
		ShaderSource Source = DEFAULT;
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
		SERIALIZABLE_PROTO(Shader, Asset);

	public:
		Shader(const Path& v, const Path& f);

		ShaderStage VertexStage = DEFAULT;
		ShaderStage FragmentStage = DEFAULT;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	struct ComputeShader : public gE::Asset
	{
		SERIALIZABLE_PROTO(ComputeShader, Asset);

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
