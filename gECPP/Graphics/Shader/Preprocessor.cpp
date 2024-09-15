//
// Created by scion on 9/14/2024.
//

#include "Preprocessor.h"

#define API_DEFINE_DIRECTIVE "#define "
#define API_INCLUDE_DIRECTIVE "#include "
#define API_DEFINE_DIRECTIVE_LENGTH 10

#define GL_VERSION_DIRECTIVE "#version 460 core\n"
#define GL_BINDLESS_DIRECTIVE "#define EXT_BINDLESS\n"
#define GL_EXTENSION_DIRECTIVE "#extension "

namespace GPU
{
	void PreprocessorPair::Write(std::string& out) const
	{
		out += API_DEFINE_DIRECTIVE;
		out += Name;
		out += ' ';
		out += Value;
		out += '\n';
	}

	void CompileDirectives(const Array<PreprocessorPair*>& pairs, std::string& out)
	{
		for(u64 i = 0; i < pairs.Count(); i++) pairs[i]->Write(out);
	}

	void CompileIncludes(const char* file, std::string& dst, std::string& directives, std::string& includes)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		char* source = (char*) ReadFileBinary(file);
		if(!source) return;
		char* line = source;

		if(includes.find(file) != std::string::npos) return;
		includes += file;

		do
		{
			if(strcmpb(line, API_INCLUDE_DIRECTIVE))
			{
				const char* includePath = GetIncludePath(file, &line[9]);
				CompileIncludes(includePath, dst, directives, includes);
				delete[] includePath;
			}
		#if API_ID == API_GL
			else if(strcmpb(line, GL_EXTENSION_DIRECTIVE))
			{
				size_t len = strlenc(line, '\n');
				directives.append(line, strlenc(line, '\n') + (line[len] != 0));
			}
		#endif
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
		if(*include == '"')
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

	void CompileShaderType(ShaderStageType stage, std::string& out)
	{
		switch(stage)
		{
		case ShaderStageType::Fragment: out += "#define FRAGMENT_SHADER\n";
		case ShaderStageType::Vertex: out += "#define VERTEX_SHADER\n";
		case ShaderStageType::Compute: out += "#define COMPUTE_SHADER\n";
		}
	}
}