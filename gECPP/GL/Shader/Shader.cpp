//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>
#include <cstring>

#define DEFINE_DIRECTIVE_LEN 9 // '#define '(8) + '\n'(1)
char DEFINE_DIRECTIVE[] = "#define ";
char VERSION_DIRECTIVE[] = "#version 460 core\n";

namespace GL
{
	template<typename T>
	bool GetShaderStatus(const T& shader, const char* source = nullptr);

	const char* CompileDirectives(const PreprocessorPair* pairs, u8 count);

	void Shader::SetUniform(u8 loc, const Texture* tex, u8 slot) const
	{
		if(!tex) return;
		SetUniform(loc, i32(tex->Use(slot)));
	}

	Shader::Shader(gE::Window* window, const char* v, const char* f, const PreprocessorPair* p, u8 c) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage frag(window, Fragment, f, p, c), vert(window, Vertex, v, p, c);
		frag.Attach(this);
		vert.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ShaderStage::ShaderStage(gE::Window* window, ShaderStageType type, const char* file, const PreprocessorPair* directives, u8 dLen) : Asset(window)
	{
		ID = glCreateShader(type);

		const char* source = (char*) ReadFile(file, true);
		const char* directivesSrc = CompileDirectives(directives, dLen);
		const char* sources[] {VERSION_DIRECTIVE, directivesSrc, source};

		glShaderSource(ID, 3, sources, nullptr);
		glCompileShader(ID);

		GetShaderStatus(*this, source);

		delete[] source;
		delete[] directivesSrc;
	}

	Shader::Shader(gE::Window* window, const ShaderStage& v, const ShaderStage& f) : Asset(window)
	{
		ID = glCreateProgram();

		v.Attach(this);
		f.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	Shader::Shader(gE::Window* window, const char* src , const GL::PreprocessorPair* pairs, u8 count) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage c(window, Compute, src, pairs, count);
		c.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	template<typename T>
	bool GetShaderStatus(const T& shader, const char* source)
	{
		u32 id = shader.Get();

		i32 shaderStatus;
		if constexpr (std::is_same_v<T, ShaderStage>)
			glGetShaderiv(id, GL_COMPILE_STATUS, &shaderStatus);
		else
			glGetProgramiv(id, GL_LINK_STATUS, &shaderStatus);

		if (shaderStatus) return true;

		if constexpr (std::is_same_v<T, ShaderStage>)
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &shaderStatus);
		else
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &shaderStatus);

		char* infoLog = new char[shaderStatus] {};
		glGetProgramInfoLog(id, shaderStatus - 1, nullptr, infoLog);
		std::cout << "SHADER COMPILE FAILURE: " << infoLog << '\n';
		std::cout << source << std::endl;

		delete[] infoLog;
		return false;
	}

	const char* CompileDirectives(const PreprocessorPair* pairs, u8 count)
	{
		u32 directiveSrcLen = 0;
		for(u8 i = 0; i < count; i++)
			directiveSrcLen += DEFINE_DIRECTIVE_LEN + pairs[i].TotalLength;

		char* directivesSrc = new char[directiveSrcLen + 1];
		directivesSrc[directiveSrcLen] = 0;

		char* currentDirective = directivesSrc;
		for(u8 i = 0; i < count; i++)
		{
			const PreprocessorPair& directive = pairs[i];

			memcpy(currentDirective, DEFINE_DIRECTIVE, 8);
			currentDirective += 8;

			memcpy(currentDirective, directive.Name, directive.NameLength);
			currentDirective += directive.NameLength;

			*currentDirective = ' ';
			currentDirective++;

			memcpy(currentDirective, directive.Value, directive.ValueLength);
			currentDirective += directive.ValueLength;

			*currentDirective = '\n';
			currentDirective++;
		}

		return directivesSrc;
	}
}