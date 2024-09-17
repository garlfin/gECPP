//
// Created by scion on 9/14/2024.
//

#include "Preprocessor.h"

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

	void CompileIncludes(std::istream& source, std::string& extensions, std::string& out)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		for(std::string line; std::getline(source, line);)
		{
			if(strcmpb(line.c_str(), API_INCLUDE_DIRECTIVE))
			{
				std::string includePath = GetIncludePath(line);
				std::ifstream file = std::ifstream(includePath, std::ios_base::in | std::ios_base::binary);
				CompileIncludes(file, extensions, out);
			}
		#if API_ID == API_GL
			else if(strcmpb(line.c_str(), GL_EXTENSION_DIRECTIVE)) extensions += line;
		#endif
			else out += line;
		}

		out += '\n';
	}

	std::string GetIncludePath(const std::string& line)
	{
		char delimiter;
		std::string path;
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