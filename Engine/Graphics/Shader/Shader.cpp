//
// Created by scion on 9/13/2024.
//

#include "Shader.h"

#include <Window.h>

namespace GPU
{
	ShaderSource::ShaderSource(const Path& path)
	{
		Source = ReadFile(path);
	}

	void ShaderSource::IDeserialize(istream& in, SETTINGS_T s)
	{
		Read(in, Source);
	}

	void ShaderSource::ISerialize(ostream& out) const
	{
		Write(out, Source);
	}

	REFLECTABLE_FACTORY_IMPL(ShaderSource, ShaderSource);
	REFLECTABLE_ONGUI_IMPL(ShaderSource,
		const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
		ImVec2 size = ImGui::CalcTextSize(&*Source.begin(), &*Source.end(), contentRegion.x);
		size.x = std::max(size.x, contentRegion.x);
		size.y = std::min(size.y, contentRegion.y);

		ImGui::InputTextMultiline("##source", &Source, size, ImGuiInputTextFlags_NoHorizontalScroll);
	);

	void ShaderStage::IDeserialize(istream& in, SETTINGS_T s)
	{
		StageType = Read<ShaderStageType>(in);
		ReadSerializable(in, Source, s);
	}

	void ShaderStage::ISerialize(ostream& out) const
	{
		Write(out, StageType);
		Write(out, Source);
	}

	void Shader::IDeserialize(istream& in, SETTINGS_T s)
	{
		ReadSerializable(in, VertexStage, s);
		ReadSerializable(in, FragmentStage, s);
	}

	void Shader::ISerialize(ostream& out) const
	{
		Write(out, VertexStage);
		Write(out, FragmentStage);
	}

	void ComputeShader::IDeserialize(istream& in, SETTINGS_T s)
	{
		ReadSerializable(in, ComputeStage, s);
	}

	void ComputeShader::ISerialize(ostream& out) const
	{
		Write(out, ComputeStage);
	}

	ShaderStage::ShaderStage(ShaderStageType type, const Path& path) :
		StageType(type),
		Source(path),
		BasePath(path)
	{
	}

	REFLECTABLE_ONGUI_IMPL(ShaderStage,
		gE::DrawField<const ShaderStageType>(gE::EnumField{ "Type", "", EShaderStageType }, StageType, depth);
		gE::DrawField(gE::Field{ "Source" }, Source, depth);
		gE::DrawField<const std::string>(gE::Field{ "Base Path", "The path used for relative includes (#include \"\")." }, BasePath.string(), depth);
	);
	REFLECTABLE_FACTORY_IMPL(ShaderStage, API::ShaderStage)

	Shader::Shader(const Path& v, const Path& f) :
		VertexStage(ShaderStageType::Vertex, v),
		FragmentStage(ShaderStageType::Fragment, f)
	{
	}

	REFLECTABLE_ONGUI_IMPL(Shader,
		gE::DrawField(gE::Field{ "Vertex" }, VertexStage, depth);
		gE::DrawField(gE::Field{ "Fragment" }, FragmentStage, depth);
	);
	REFLECTABLE_FACTORY_IMPL(Shader, API::Shader);

	ComputeShader::ComputeShader(const Path& c) :
		ComputeStage(ShaderStageType::Compute, c)
	{
	};

	REFLECTABLE_ONGUI_IMPL(ComputeShader,
		gE::DrawField(gE::Field{ "Compute" }, ComputeStage, depth);
	);
	REFLECTABLE_FACTORY_IMPL(ComputeShader, API::ComputeShader);
}
