//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>
#include <cstring>

#define DEFINE_DIRECTIVE_LEN 10
char DEFINE_DIRECTIVE[] = "#define ";
char VERSION_DIRECTIVE[] = "#version 460 core\n";

namespace GL
{
	void GetShaderStatus(const Shader& shader);

	void Shader::SetUniform(u8 loc, const Texture* tex, u8 slot) const
	{
		if(!tex) return;
		SetUniform(loc, i32(tex->Use(slot)));
	}

	Shader::Shader(gE::Window* window, const char* v, const char* f, PreprocessorPair* p, u8 c) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage frag(window, Fragment, f, p, c), vert(window, Vertex, v, p, c);
		frag.Attach(this);
		vert.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ShaderStage::ShaderStage(gE::Window* window, ShaderStageType type, const char* file, PreprocessorPair* pairs, u8 pLen) : Asset(window)
	{
		ID = glCreateShader(type);

		// TODO: COMPLETE INCLUDES AND DIRECTIVES

		const char* f[] { VERSION_DIRECTIVE, (char*) ReadFile(file) };
		glShaderSource(ID, 2, f, nullptr);

		delete[] f[1]; /// deletes source

		glCompileShader(ID);

		i32 shaderStatus;
		glGetShaderiv(ID, GL_COMPILE_STATUS, &shaderStatus);

		if(shaderStatus) return;

		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &shaderStatus);
		char* infoLog = new char[shaderStatus] {};
		glGetShaderInfoLog(ID, shaderStatus - 1, nullptr, infoLog);
		std::cout << "STAGE COMPILE FAILURE: \n" << infoLog << std::endl;
	}

	Shader::Shader(gE::Window* window, const ShaderStage& v, const ShaderStage& f, PreprocessorPair*, u8) : Asset(window)
	{
		ID = glCreateProgram();

		v.Attach(this);
		f.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	Shader::Shader(gE::Window* window, const char* src , GL::PreprocessorPair* pairs, u8 count) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage c(window, Compute, src, pairs, count);
		c.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	void GetShaderStatus(const Shader& shader)
	{
		u32 id = shader.Get();

		i32 shaderStatus;
		glGetProgramiv(id, GL_LINK_STATUS, &shaderStatus);

		if(shaderStatus) return;

		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &shaderStatus);
		char* infoLog = new char[shaderStatus] {};
		glGetProgramInfoLog(id, shaderStatus - 1, nullptr, infoLog);
		std::cout << "SHADER COMPILE FAILURE: \n" << infoLog << std::endl;

		delete[] infoLog;
	}
}