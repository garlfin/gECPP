//
// Created by scion on 10/25/2023.
//

#include "Shader.h"
#include <iostream>
#include <Graphics/Texture/Texture.h>

#define INCLUDE_DIRECTIVE "#include "
#define EXTENSION_DIRECTIVE "#extension "

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>& pairs, std::string& buf);
	void CompileIncludes(const char* file, std::string& dst, std::string& directives, std::string& includes);
	const char* GetIncludePath(const char* origin, const char* include);

	void CompileDirectives(const Array<PreprocessorPair>& pairs, std::string& buf)
	{
		for(u64 i = 0; i < pairs.Count(); i++) pairs[i].Write(buf);
	}

	void CompileIncludes(const char* file, std::string& dst, std::string& directives, std::string& includes)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		char* source = (char*) ReadFile(file);
		if(!source) return;
		char* line = source;

		if(includes.find(file) != std::string::npos) return;
		includes += file;

		do
		{
			if(strcmpb(line, INCLUDE_DIRECTIVE))
			{
				const char* includePath = GetIncludePath(file, &line[9]);
				CompileIncludes(includePath, dst, directives, includes);
				delete[] includePath;
			}
			else if(strcmpb(line, EXTENSION_DIRECTIVE))
			{
				size_t len = strlenc(line, '\n');
				directives.append(line, strlenc(line, '\n') + (line[len] != 0));
			}
			else
			{
				size_t len = strlenc(line, '\n');
				dst.append(line, strlenc(line, '\n') + (line[len] != 0));
			}
		} while((line = (char*) IncrementLine(line)));

		dst += '\n';

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

			char* path = new char[totalLen + 1]{};

			// Original Copy
			memcpy(path, origin, filelessLen);
			memcpy(path + filelessLen, include + 1, includeLen);

			// Canonicalization
			// TODO
			// Fr lazy atm

			return path;
		}

		LOG("WARNING: INVALID DELIMITER\n\tDELIMITER: " << *include);
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

	PreprocessorPair::PreprocessorPair(const PreprocessorPair& o)
		: Name(), Value(Name + (o.Value - o.Name))
	{
		u32 len = strlen(o.Name) + 1;
		if(o.Value) len += strlen(o.Value) + 1;
		else Value = nullptr;

		Name = new char[len];
		memcpy(Name, o.Name, len);
	}

	void PreprocessorPair::Write(std::string& buf) const
	{
		if(!Name) return;

		buf += "#define ";
		buf += Name;

		if(Value)
		{
			buf += ' ';
			buf += Value;
		}

		buf += '\n';
	}

	const char* ShaderStageDefine(ShaderStageType type)
	{
		switch(type)
		{
		case Fragment: return "#define FRAGMENT_SHADER\n";
		case Vertex: return "#define VERTEX_SHADER\n";
		case Compute: return "#define COMPUTE_SHADER\n";
		default: return "";
		}
	}
}