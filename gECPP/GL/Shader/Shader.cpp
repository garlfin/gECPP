//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>

#define DEFINE_DIRECTIVE "#define "
#define INCLUDE_DIRECTIVE "#include "
#define VERSION_DIRECTIVE "#version 460 core\n"
#define EXTENSION_DIRECTIVE "#extension "

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>*, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&, gETF::SerializationBuffer&);
	const char* GetIncludePath(const char* origin, const char* include);

	template<typename T>
	bool GetShaderStatus(const T& shader, char* source = nullptr);

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
#ifdef DEBUG
		GetShaderStatus(*this);
#endif
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
#ifdef DEBUG
		GetShaderStatus(*this, bufPtr);
#endif
	}

	Shader::Shader(gE::Window* window, const ShaderStage& v, const ShaderStage& f) : Asset(window)
	{
		ID = glCreateProgram();

		v.Attach(this);
		f.Attach(this);

		glLinkProgram(ID);

#ifdef DEBUG
		GetShaderStatus(*this);
#endif
	}

	Shader::Shader(gE::Window* window, const char* src, const Array<PreprocessorPair>* p) : Asset(window)
	{
		ID = glCreateProgram();

		ShaderStage c(window, Compute, src, p);
		c.Attach(this);

		glLinkProgram(ID);
#ifdef DEBUG
		GetShaderStatus(*this);
#endif
	}

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ARGS...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(ARGS); else FUNC_SHADER(ARGS);

	template<typename T>
	bool GetShaderStatus(const T& shader, char* source)
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
		if(source)
		{
			std::cout << "0(1): ";
			u32 i = 2;
			for(;*source; source++)
			{
				if (*source == '\n')
				{
					std::cout << "\n0(" << i << "): ";
					i++;
				}
				else std::cout << *source;

				if(!source[1]) std::cout << '\n';
			}
		}

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

			if(directive.Value)
			{
				buf.Push(' ');
				buf.StrCat(directive.Value);
			}

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
			}
			else if (StrCmp(line, EXTENSION_DIRECTIVE)) directivesBuffer.StrCat(line, '\n', 1);
			else dstBuffer.StrCat(line, '\n', 1);
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

			char* path = new char[totalLen + 1];
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
		u32 valueLength = v ? strlen(v) : 0;
		u32 totalLength = nameLength + valueLength + 2; // 2 null terminators
		Name = new char[totalLength];
		Value = Name + nameLength;
		memcpy(Name, n, nameLength + 1);
		if(valueLength) memcpy(Value, v, valueLength + 1);
	}

	PreprocessorPair::PreprocessorPair(const PreprocessorPair& o):
		Name(), Value(Name + (o.Value - o.Name))
	{
		u32 len = strlen(o.Name) + strlen(o.Value) + 2; // 2 terminators
		Name = new char[len];
		memcpy(Name, o.Name, len);
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

