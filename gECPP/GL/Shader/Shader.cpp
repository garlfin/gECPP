//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>

char DEFINE_DIRECTIVE[] = "#define ";
char INCLUDE_DIRECTIVE[] = "#include ";
char VERSION_DIRECTIVE[] = "#version 460 core\n";

namespace GL
{
	template<typename T>
	bool GetShaderStatus(const T& shader, const char* source = nullptr);
	void CompileDirectives(const PreprocessorPair* pairs, u8 count, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&);

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

		gETF::SerializationBuffer sourceBuf{};

		sourceBuf.StrCat(VERSION_DIRECTIVE);
		CompileDirectives(directives, dLen, sourceBuf);
		CompileIncludes(file, sourceBuf);

		char* bufPtr = (char*) sourceBuf.Data();

		glShaderSource(ID, 1, &bufPtr, nullptr);
		glCompileShader(ID);

		GetShaderStatus(*this, bufPtr);
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

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ARGS...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(ARGS); else FUNC_SHADER(ARGS);

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

		GL_GET(glGetShaderiv, glGetProgramiv, id, GL_INFO_LOG_LENGTH, &shaderStatus);

		char* infoLog = new char[shaderStatus] {};

		GL_GET(glGetShaderInfoLog, glGetProgramInfoLog, id, shaderStatus - 1, nullptr, infoLog);

		std::cout << "SHADER COMPILE FAILURE:\n" << infoLog << '\n';
		if(source) std::cout << "SOURCE:\n" << source << std::endl;

		delete[] infoLog;
		return false;
	}

	void CompileDirectives(const PreprocessorPair* pairs, u8 count, gETF::SerializationBuffer& buf)
	{
		for(u8 i = 0; i < count; i++)
		{
			const PreprocessorPair& directive = pairs[i];

			buf.StrCat(DEFINE_DIRECTIVE);
			buf.StrCat(directive.Name);
			buf.StrCat(" ");
			buf.StrCat(directive.Value);
			buf.StrCat("\n");
		}
	}

	void CompileIncludes(const char* file, gETF::SerializationBuffer& dstBuffer)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		// already wrote all this,
		char* source = (char*) ReadFile(file);
		if(!source) return;
		char* line = source;

		do
		{
			if(!StrCmp(line, INCLUDE_DIRECTIVE))
			{
				dstBuffer.StrCat(line, '\n');
				dstBuffer.StrCat("\n");
				continue;
			}

			char delimiter = '"';
			char* include;
			if(!(include = (char*) IncrementLine(line, '"')))
			{
				delimiter = '>';
				include = (char*) IncrementLine(line, '<');
			}

			size_t filenameLen = strlenc(include, delimiter) - 1;

			include[filenameLen] = 0; // cut off to load file
			CompileIncludes(include, dstBuffer);
			include[filenameLen] = delimiter; // restore


		} while ((line = (char*) IncrementLine(line)));

		delete[] source;
	}
}