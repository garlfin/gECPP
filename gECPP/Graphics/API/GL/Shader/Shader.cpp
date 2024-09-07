//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include "Uniform.h"
#include "Graphics/Texture/Texture.h"
#include <iostream>
#include "Engine/Window.h"

#define VERSION_DIRECTIVE "#version 460 core\n"
#define EXT_BINDLESS "#define EXT_BINDLESS\n"
// Weird AMD driver bug doesn't define GL_ARB_bindless_texture

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ARGS...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(ARGS); else FUNC_SHADER(ARGS);

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>& src, std::string& dst);
	void CompileIncludes(const char* file, std::string& dst, std::string& directives, std::string& includes);
	const char* GetIncludePath(const char* origin, const char* include);

	template<typename T>
	bool GetShaderStatus(const T& shader, const char* name = nullptr, const char* source = nullptr);

	Shader::Shader(gE::Window* window, const char* v, const char* f, const Array<PreprocessorPair>* p) : APIObject(window)
	{
		ID = glCreateProgram();

		ShaderStage frag(window, Fragment, f, p), vert(window, Vertex, v, p);
		frag.Attach(this);
		vert.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ShaderStage::ShaderStage(gE::Window* window, ShaderStageType type, const char* file, const Array<PreprocessorPair>* pairs)
		: APIObject(window)
	{
		ID = glCreateShader(type);

		std::string source, directives, includes;

		directives += VERSION_DIRECTIVE;

		CompileIncludes(file, source, directives, includes);
		if(pairs) CompileDirectives(*pairs, directives);
		if(GLAD_GL_ARB_bindless_texture) directives += EXT_BINDLESS;
		directives += ShaderStageDefine(type);
		directives += source;

		const char* src = directives.data();

		glShaderSource(ID, 1, &src, nullptr);
		glCompileShader(ID);

		GetShaderStatus(*this, file, src);
	}

	Shader::Shader(gE::Window* window, const ShaderStage& v, const ShaderStage& f) : APIObject(window)
	{
		ID = glCreateProgram();

		v.Attach(this);
		f.Attach(this);

		glLinkProgram(ID);

#ifdef DEBUG
		GetShaderStatus(*this);
#endif
	}

	Shader::Shader(gE::Window* window, const char* src, const Array<PreprocessorPair>* p) : APIObject(window)
	{
		ID = glCreateProgram();

		ShaderStage c(window, Compute, src, p);
		c.Attach(this);

		glLinkProgram(ID);
#ifdef DEBUG
		GetShaderStatus(*this);
#endif
	}

	template<typename T>
	bool GetShaderStatus(const T& shader, const char* name, const char* source)
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

	void Shader::SetUniform(u8 loc, const Texture& tex, u8 slot) const
	{
		SetUniform(loc, tex.Use(slot));
	}

	void Shader::SetUniform(u8 loc, const Texture& tex) const
	{
		SetUniform(loc, GetWindow().GetSlotManager().Increment(&tex));
	}

	DynamicUniform::DynamicUniform(Shader* s, u32 l) : _shader(s), _location(l) { }
	DynamicUniform::DynamicUniform(Shader* s, const char* n) : _shader(s), _location(GetUniformLocation(n)) { }

	template<>
	void DynamicUniform::Set(const Texture& t) const
	{
		_shader->SetUniform(_location, t, _shader->GetWindow().GetSlotManager().Increment(&t));
	}
}

