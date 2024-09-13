//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>

namespace GPU
{
	class ShaderSource;

	class ShaderStage : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(ShaderStage, Serializable);

	public:
		GLenum Type;
		std::string Source;

		ALWAYS_INLINE void Free() override { Source.clear(); Source.shrink_to_fit(); };
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Source.data(); }
	};

	class Shader : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(Shader, Serializable);

	public:
		ShaderStage VertexStage;
		ShaderStage FragmentStage;

		ALWAYS_INLINE void Free() override { VertexStage.Free(); FragmentStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return VertexStage.IsFree() && FragmentStage.IsFree(); }
	};

	class ComputeShader : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO_T(ComputeShader, Serializable);

	public:
		ShaderStage ComputeStage;

		ALWAYS_INLINE void Free() override { return ComputeStage.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return ComputeStage.IsFree(); }
	};
}

#include <Graphics/API/GL/Shader/Shader.h>