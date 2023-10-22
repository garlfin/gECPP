//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include <GL/Texture/Texture.h>
#include <iostream>


#define INCLUDE_DIRECTIVE "#include "
#define VERSION_DIRECTIVE "#version 460 core\n"
#define EXTENSION_DIRECTIVE "#extension "

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ARGS...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(ARGS); else FUNC_SHADER(ARGS);

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>*, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&, gETF::SerializationBuffer&, gETF::SerializationBuffer& idBuffer);
	const char* GetIncludePath(const char* origin, const char* include);

	template<typename T>
	bool GetShaderStatus(const T& shader, char* source = nullptr);

	void Shader::SetUniform(u8 loc, const Texture& tex, u8 slot) const
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
		gETF::SerializationBuffer incIDBuf{};

		directivesBuf.StrCat(VERSION_DIRECTIVE, false);
		directivesBuf.StrCat(ShaderStageDefine(type), false);

		CompileDirectives(directives, directivesBuf);
		CompileIncludes(file, sourceBuf, directivesBuf, incIDBuf);
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

		#ifdef DEBUG
		if(source)
		{
			for (u32 i = 0; *source; source++)
				if(*source == '\n' || !i)
				{
					if(i) std::cout << "\n0(" << i << "): ";
					else std::cout << "0(0): ";
					i++;
				}
				else std::cout << *source;
		}
		#endif

		delete[] infoLog;
		return false;
	}

	void CompileDirectives(const Array<PreprocessorPair>* pairs, gETF::SerializationBuffer& buf)
	{
		if(!pairs) return;
		for(u64 i = 0; i < pairs->Size(); i++) (*pairs)[i].WriteDirective(buf);
	}

	void CompileIncludes(const char* file, gETF::SerializationBuffer& dstBuffer, gETF::SerializationBuffer& directivesBuffer, gETF::SerializationBuffer& idBuffer)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		char* source = (char*) ReadFile(file);
		if(!source) return;
		char* line = source;

		if(idBuffer.Find(file))
		{
			std::cout << "Include already included, skipping " << file << '\n';
			return;
		}
		idBuffer.StrCat(file);

		do
		{
			if(StrCmp(line, INCLUDE_DIRECTIVE))
			{
				const char* includePath = GetIncludePath(file, &line[9]);
				CompileIncludes(includePath, dstBuffer, directivesBuffer, idBuffer);
				delete[] includePath;
			}
			else if (StrCmp(line, EXTENSION_DIRECTIVE)) directivesBuffer.StrCat(line, true, '\n');
			else dstBuffer.StrCat(line, true, '\n');
		} while ((line = (char*) IncrementLine(line)));

		delete[] source;
	}

	const char* GetIncludePath(const char* origin, const char* include)
	{
		if(*include == '<') return strdupc(include + 1, '>');
		else if(*include == '"')
		{
			// Allocation
			size_t filelessLen = strlencLast(origin, '/') + 1;
			size_t includeLen = strlenc(include + 1, '"');
			size_t totalLen = filelessLen + includeLen;

			char* path = new char[totalLen + 1] {};

			// Original Copy
			memcpy(path, origin, filelessLen);
			memcpy(path + filelessLen, include + 1, includeLen);

			// Canonicalization
			// TODO
			// Fr lazy atm

			return path;
		}

		std::cout << "Invalid delimiter: " << *include << '\n';
		return nullptr;
	}

	PreprocessorPair::PreprocessorPair(const char* n, const char* v)
	{
		u32 totalLength = strlen(n) + 1;
		u32 nameLength = totalLength, valueLength;

		if(v) totalLength += valueLength = strlen(v) + 1;

		Name = new char[totalLength];
		Value = nullptr;
		memcpy(Name, n, nameLength);

		if(!v) return;

		Value = Name + nameLength + 1;
		memcpy(Value, v, valueLength);
	}

	PreprocessorPair::PreprocessorPair(const PreprocessorPair& o):
		Name(), Value(Name + (o.Value - o.Name))
	{
		u32 len = strlen(o.Name) + 1;
		if(o.Value) len += strlen(o.Value) + 1;
		else Value = nullptr;

		Name = new char[len];
		memcpy(Name, o.Name, len);
	}

	void PreprocessorPair::WriteDirective(gETF::SerializationBuffer& buf) const
	{
		if(!Name) return;

		buf.StrCat("#define ");
		buf.StrCat(Name);

		if(Value)
		{
			buf.Push(' ');
			buf.StrCat(Value);
		}

		buf.Push('\n');
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
			return "#define COMPUTE_SHADER\n";
		default:
			return "";
		}
	}
}

