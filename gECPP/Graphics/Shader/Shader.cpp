//
// Created by scion on 9/13/2024.
//

#include "Shader.h"

#include <Engine/Window/Window.h>

namespace GPU
{
	ShaderSource::ShaderSource(const Path& path)
	{
		Source = ReadFile(path);
	}

	void ShaderSource::ISerialize(istream& in, SETTINGS_T s)
	{
		Read(in, Source);
	}

	void ShaderSource::IDeserialize(ostream& out) const
	{
		Write(out, Source);
	}

	void ShaderStage::ISerialize(istream& in, SETTINGS_T s)
	{
		StageType = Read<ShaderStageType>(in);
		ReadSerializable(in, Source, s);
	}

	void ShaderStage::IDeserialize(ostream& out) const
	{
		Write(out, StageType);
		Write(out, Source);
	}

	void Shader::ISerialize(istream& in, SETTINGS_T s)
	{
		ReadSerializable(in, VertexStage, s);
		ReadSerializable(in, FragmentStage, s);
	}

	void Shader::IDeserialize(ostream& out) const
	{
		Write(out, VertexStage);
		Write(out, FragmentStage);
	}

	void ComputeShader::ISerialize(istream& in, SETTINGS_T s)
	{
		ReadSerializable(in, ComputeStage, s);
	}

	void ComputeShader::IDeserialize(ostream& out) const
	{
		Write(out, ComputeStage);
	}

	SERIALIZABLE_REFLECTABLE_IMPL(ShaderStage, API::ShaderStage)

	ShaderStage::ShaderStage(ShaderStageType type, const Path& path) :
		StageType(type),
		Source(path),
		BasePath(path)
	{
	}

	SERIALIZABLE_REFLECTABLE_IMPL(Shader, API::Shader)

	Shader::Shader(const Path& v, const Path& f) :
		VertexStage(ShaderStageType::Vertex, v),
		FragmentStage(ShaderStageType::Fragment, f)
	{
	}

	SERIALIZABLE_REFLECTABLE_IMPL(ComputeShader, API::ComputeShader)

	ComputeShader::ComputeShader(const Path& c) :
		ComputeStage(ShaderStageType::Compute, c)
	{
	};
}
