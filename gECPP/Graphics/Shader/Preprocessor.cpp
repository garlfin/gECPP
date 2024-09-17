//
// Created by scion on 9/14/2024.
//

#include "Preprocessor.h"

#include <filesystem>

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

	void CompileDirectives(const Array<PreprocessorPair>& pairs, std::string& out)
	{
		for(u64 i = 0; i < pairs.Count(); i++) pairs[i].Write(out);
	}

	void CompileIncludes(std::istream& source, std::string& extensions, std::string& out, const Path& path)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		for(std::string line; std::getline(source, line);)
		{
			line += '\n';
			if(strcmpb(line.c_str(), API_INCLUDE_DIRECTIVE))
			{
				Path includePath = GetIncludePath(line, path);
				std::ifstream file = std::ifstream(includePath, std::ios_base::in | std::ios_base::binary);
				if(!file.is_open()) LOG("COULD NOT FIND FILE: " << includePath);
				CompileIncludes(file, extensions, out, includePath);
			}
		#if API_ID == API_GL
			else if(strcmpb(line.c_str(), GL_EXTENSION_DIRECTIVE)) extensions += line;
		#endif
			else out += line;
		}

		out += '\n';
	}

	Path GetIncludePath(const std::string& line, const Path& path)
	{
		char delimiter = line.at(9);

		GE_ASSERT(delimiter == '"' || delimiter == '<', "INVALID INCLUDE DELIMITER!");

		if(delimiter == '<') delimiter = '>';

		const size_t end = line.find(delimiter, 10);

		GE_ASSERT(end != std::string::npos, "INVALID INCLUDE!");

		if(delimiter == '>')
			return line.substr(10, end - 10);
		return weakly_canonical(path.parent_path() / line.substr(10, end - 10));
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
