//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <iostream>
#include <sstream>
#include <Engine/Window.h>
#include <Graphics/Shader/Preprocessor.h>
#include <Graphics/Texture/Texture.h>

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(__VA_ARGS__); else FUNC_SHADER(__VA_ARGS__);

namespace GL
{
	template<typename T>
	bool GetShaderStatus(const T& shader, const std::string& name = {}, const char* source = nullptr);

	IShader::IShader(gE::Window* window) : APIObject(window)
	{
		ID = glCreateProgram();
	}

	void IShader::SetUniform(u8 loc, const Texture& tex, u8 slot) const
	{
		SetUniform(loc, tex.Use(slot));
	}

	void IShader::SetUniform(u8 loc, const Texture& tex) const
	{
		SetUniform(loc, GetWindow().GetSlotManager().Increment(&tex));
	}

	Shader::Shader(gE::Window* window, const std::string& vertPath, const std::string& fragPath) : IShader(window)
	{
		const ShaderStage vert(window, GPU::ShaderStageType::Vertex, vertPath);
		const ShaderStage frag(window, GPU::ShaderStageType::Fragment, fragPath);

		frag.Attach(*this);
		vert.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	Shader::Shader(gE::Window* window, const ShaderStage& vert, const ShaderStage& frag) : IShader(window)
	{
		vert.Attach(*this);
		frag.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	Shader::Shader(gE::Window* window, SUPER&& settings) : SUPER(MOVE(settings)), IShader(window)
	{
		const ShaderStage frag(window, MOVE(settings.FragmentStage));
		const ShaderStage vert(window, MOVE(settings.VertexStage));

		frag.Attach(*this);
		vert.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ComputeShader::ComputeShader(gE::Window* window, const std::string& compPath)
	{
		const ShaderStage comp(window, GPU::ShaderStageType::Compute, compPath);

		comp.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ComputeShader::ComputeShader(gE::Window* window, const ShaderStage& comp) : IShader(window)
	{
		comp.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ComputeShader::ComputeShader(gE::Window* window, SUPER&& settings) : SUPER(MOVE(settings)), IShader(window)
	{
		const ShaderStage comp(window, MOVE(settings.ComputeStage));

		comp.Attach(*this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ShaderStage::ShaderStage(gE::Window* window, GPU::ShaderStageType stage, const std::string& path) : APIObject(window)
	{
		std::string source, extensions;
		std::ifstream file = std::ifstream(path, std::ios_base::in | std::ios_base::binary);

		source += GL_VERSION_DIRECTIVE;
		CompileShaderType(stage, source);
		CompileDirectives(window->GetShaderCompilationState(), source);
		GPU::CompileIncludes(file, extensions, source);

		const char* sourceCString = source.data();
		const i32 sourceLength = source.length();

		glShaderSource(ID, 1, &sourceCString, &sourceLength);
		glCompileShader(ID);

		GetShaderStatus(*this, path, sourceCString);
	}

	ShaderStage::ShaderStage(gE::Window* window, SUPER&& settings) : SUPER(MOVE(settings)), APIObject(window)
	{
		std::string source, extensions;
		std::istringstream stream = std::istringstream(Source);

		source += GL_VERSION_DIRECTIVE;
		CompileShaderType(Type, source);
		CompileDirectives(window->GetShaderCompilationState(), source);
		GPU::CompileIncludes(stream, extensions, source);

		const char* sourceCString = source.data();
		const i32 sourceLength = source.length();

		glShaderSource(ID, 1, &sourceCString, &sourceLength);
		glCompileShader(ID);

		GetShaderStatus(*this, "", sourceCString);
	}

	template<typename T>
	bool GetShaderStatus(const T& shader, const std::string& name, const char* source)
	{
		u32 id = shader.Get();

		i32 shaderStatus;
		if constexpr(std::is_same_v<T, ShaderStage>)
			glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus);
		else
			glGetProgramiv(id, GL_LINK_STATUS, &shaderStatus);

		if(shaderStatus) return true;

		GL_GET(glGetShaderiv, glGetProgramiv, id, GL_INFO_LOG_LENGTH, &shaderStatus);

		char* infoLog = new char[shaderStatus]{};

		GL_GET(glGetShaderInfoLog, glGetProgramInfoLog, id, shaderStatus - 1, nullptr, infoLog);

	#ifdef DEBUG
		if(source)
		{
			std::cout << "FILE: " << name << '\n';

			std::string debugBuffer;
			for(u32 i = 1; *source; i++)
			{
				debugBuffer += "0(" + std::to_string(i) + "): ";

				size_t len = strlenc(source, '\n') + 1;
				debugBuffer.append(source, len);

				source += len;
			}

			std::cout << debugBuffer;
		}
		std::cout << "SHADER COMPILE FAILURE:\n" << infoLog << '\n';
	#endif

		delete[] infoLog;
		return false;
	}
}