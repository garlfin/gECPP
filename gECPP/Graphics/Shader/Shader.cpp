//
// Created by scion on 9/13/2024.
//

#include "Shader.h"
#include "Uniform.h"

#include <Engine/Window.h>

namespace GPU
{
	void ShaderStage::ISerialize(istream& in, SETTINGS_T s)
	{
		Type = Read<ShaderStageType>(in);
		Read<std::string>(in, Source);
	}

	void ShaderStage::IDeserialize(ostream& out) const
	{
		Write(out, Type);
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

	DynamicUniform::DynamicUniform(API::Shader* shader, u32 location) :
		_shader(shader), _location(location)
	{ }
	DynamicUniform::DynamicUniform(API::Shader* shader, const std::string& name) :
		_shader(shader), _location(shader->GetUniformLocation(name))
	{ }

	template<>
	void DynamicUniform::Set(const API::Texture& t) const
	{
		_shader->SetUniform(_location, t, _shader->GetWindow().GetSlotManager().Increment(&t));
	}
}
