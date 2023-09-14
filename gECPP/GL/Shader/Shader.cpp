//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>

char DEFINE_DIRECTIVE[] = "#define ";
char INCLUDE_DIRECTIVE[] = "#include ";
char VERSION_DIRECTIVE[] = "#version 460 core\n";
char EXTENSION_DIRECTIVE[] = "#extension ";
char EXTENSION_DIRECTIVE_END[] = " : require";
char EXTENSION_REQUIRE_DIRECTIVE[] = "#require\n";

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>*, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&, gETF::SerializationBuffer&);
	const char* GetIncludePath(const char* origin, const char* include);

	template<typename T>
	bool GetShaderStatus(const T& shader, const char* source = nullptr);

	void Shader::SetUniform(u8 loc, const TextureHandle& tex, u8 slot) const
	{
		SetUniform(loc, i32(tex.Use(slot)));
	}

	Shader::Shader(gE::Window* window, const char* v, const char* f, const Array<PreprocessorPair>* p) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage frag(window, Fragment, f, p), vert(window, Vertex, v, p);
		frag.Attach(this);
		vert.Attach(this);

		glLinkProgram(ID);

		GetShaderStatus(*this);
	}

	ShaderStage::ShaderStage(gE::Window* window, ShaderStageType type, const char* file, const Array<PreprocessorPair>* directives) : Asset(window)
	{
		ID = glCreateShader(type);

		gETF::SerializationBuffer directivesBuf{};
		gETF::SerializationBuffer sourceBuf{};

		directivesBuf.StrCat(VERSION_DIRECTIVE);
		directivesBuf.StrCat(ShaderStageDefine(type));
		CompileDirectives(directives, directivesBuf);
		CompileIncludes(file, sourceBuf, directivesBuf);
		directivesBuf.Push(sourceBuf);
		directivesBuf.Push('\0');

		char* bufPtr = (char*) directivesBuf.Data();

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

	Shader::Shader(gE::Window* window, const char* src, const Array<PreprocessorPair>* p) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage c(window, Compute, src, p);
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

	void CompileDirectives(const Array<PreprocessorPair>* pairs, gETF::SerializationBuffer& buf)
	{
		if(!pairs) return;

		for(u64 i = 0; i < pairs->Size(); i++)
		{
			const PreprocessorPair& directive = (*pairs)[i];

			buf.StrCat(DEFINE_DIRECTIVE);
			buf.StrCat(directive.Name);

			if(!directive.Value)
			{
				buf.Push('\n');
				continue;
			}

			buf.Push(' ');
			buf.StrCat(directive.Value);
			buf.Push('\n');
		}
	}

	void CompileIncludes(const char* file, gETF::SerializationBuffer& dstBuffer, gETF::SerializationBuffer& directivesBuffer)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		char* source = (char*) ReadFile(file);
		if(!source) return;
		char* line = source;

		do
		{
			if(StrCmp(line, INCLUDE_DIRECTIVE))
			{
				const char* includePath = GetIncludePath(file, &line[9]);
				CompileIncludes(includePath, dstBuffer, directivesBuffer);
				delete[] includePath;
				continue;
			}
			else if (StrCmp(line, EXTENSION_REQUIRE_DIRECTIVE))
			{
				directivesBuffer.StrCat(EXTENSION_DIRECTIVE);
				directivesBuffer.StrCat(line + 8, '\n');
				directivesBuffer.StrCat(EXTENSION_DIRECTIVE_END);
				continue;
			}

			dstBuffer.StrCat(line, '\n', 1);
		} while ((line = (char*) IncrementLine(line)));

		delete[] source;
	}

	const char* GetIncludePath(const char* origin, const char* include)
	{
		if(*include == '<') return strdupc(include + 1, '>');
		else if(*include == '"')
		{
			size_t filelessLen = strlencLast(origin, '/') + 1;
			size_t includeLen = strlenc(include + 1, '"');
			size_t totalLen = filelessLen + includeLen;

			char* path = new char[totalLen];
			path[totalLen] = 0;

			memcpy(path, origin, filelessLen);
			memcpy(path + filelessLen, include + 1, includeLen);

			return path;
		}

		std::cout << "Invalid delimiter: " << *include << '\n';
		return nullptr;
	}

	PreprocessorPair::PreprocessorPair(const char* n, const char* v)
	{
		u32 nameLength = strlen(n);
		u32 valueLength = strlen(v);
		u32 totalLength = nameLength + valueLength + 2; // 2 null terminators
		Name = new char[totalLength];
		Value = Name + nameLength;
		memcpy(Name, n, nameLength + 1);
		memcpy(Value, v, valueLength + 1);
	}

	const char* ShaderStageDefine(ShaderStageType type)
	{
		switch(type)
		{
		case Fragment:
			return "#define FRAGMENT_SHADER\n";
		case Vertex:
			return "#define VERTEX_SHADER\n";
		case Compute:
			return "#define COMPUTE_SHADER";
		default:
			return "";
		}
	}
}

