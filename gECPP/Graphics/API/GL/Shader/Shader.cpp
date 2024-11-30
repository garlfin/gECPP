//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <iostream>
#include <sstream>
#include <Engine/Window/Window.h>
#include <Graphics/Shader/Preprocessor.h>
#include <Graphics/Texture/Texture.h>

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(__VA_ARGS__); else FUNC_SHADER(__VA_ARGS__);

namespace GL
{
	template<typename T>
	bool GetShaderStatus(const T& shader, const Path& name = DEFAULT, const char* source = DEFAULT);

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

	API_SERIALIZABLE_IMPL(Shader), IShader(window)
	{
		ShaderStage frag(window, move(FragmentStage));
		ShaderStage vert(window, move(VertexStage));

		frag.Attach(*this);
		FragmentStage = move(frag.GetSettings());

		vert.Attach(*this);
		VertexStage = move(vert.GetSettings());

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	API_SERIALIZABLE_IMPL(ComputeShader), IShader(window)
	{
		ShaderStage comp(window, move(ComputeStage));

		comp.Attach(*this);
		ComputeStage = move(comp.GetSettings());

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	API_SERIALIZABLE_IMPL(ShaderStage), APIObject(window)
	{
		ID = glCreateShader((GLenum) StageType);

		std::string finalSource, source, extensions;
		std::string& includes = finalSource; // temp alias
		auto stream = std::istringstream(Source);

		GPU::CompileIncludes(stream, extensions, source, includes, BasePath);

		finalSource.clear();
		finalSource += GL_VERSION_DIRECTIVE;
		CompileShaderType(StageType, finalSource);
		CompileDirectives(window->GetShaderCompilationState(), finalSource);
		finalSource += extensions;
		finalSource += source;

		const char* sourceCString = finalSource.data();
		const i32 sourceLength = finalSource.length();

		glShaderSource(ID, 1, &sourceCString, &sourceLength);
		glCompileShader(ID);

		GetShaderStatus(*this, BasePath, sourceCString);
	}

	template<typename T>
	bool GetShaderStatus(const T& shader, const Path& name, const char* source)
	{
		u32 id = shader.Get();

		i32 shaderStatus;
		if constexpr(std::is_same_v<T, ShaderStage>)
			glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus);
		else
			glGetProgramiv(id, GL_LINK_STATUS, &shaderStatus);

		if(shaderStatus) return true;

		GL_GET(glGetShaderiv, glGetProgramiv, id, GL_INFO_LOG_LENGTH, &shaderStatus);

		auto* infoLog = new char[shaderStatus]{};

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