//
// Created by scion on 9/14/2024.
//

#include "Preprocessor.h"

#include <filesystem>
#include <format>
#include <Window.h>
#include <Core/Pointer.h>

namespace GPU
{
	void PreprocessorPair::Write(std::string& out) const
	{
		out += std::format("#define {} {}\n", Name, Value);
	}

	void CompileDirectives(const std::vector<PreprocessorPair>& pairs, std::string& out)
	{
		for(const PreprocessorPair& pair : pairs) pair.Write(out);
	}

	void CompileIncludes(gE::Window* window, std::istream& source, std::string& extensions, std::string& out, std::vector<gE::UUID>& includes, const Path& path)
	{
		// Turns out most drivers support GL_ARB_SHADER_LANGUAGE_INCLUDE
		for(std::string line; std::getline(source, line);)
		{
			line += '\n';
			if(strcmpb(line.c_str(), API_INCLUDE_DIRECTIVE))
			{
				const Path includePath = std::filesystem::relative(GetIncludePath(line, path));
				const gE::UUID includeUUID = gE::HashPath(includePath);
				const gE::File* file = window->GetAssets().FindFile(includeUUID);

				if(!file)
				{
					gE::Reference<gE::Asset> asset = gE::ref_create<ShaderSource>(includePath);
					file = window->GetAssets().AddFile(std::move(gE::File(includePath, asset)));
				}

				if(!file) continue;

				std::istringstream includeStream(file->Cast<ShaderSource, false>()->Source);

				if(std::ranges::find(includes, includeUUID) != includes.end())
					continue;

			#ifdef DEBUG
				out += "// BEGIN " + includePath.string() + '\n';
			#endif

				includes.push_back(includeUUID);
				CompileIncludes(window, includeStream, extensions, out, includes, includePath);

			#ifdef DEBUG
				out += "// END " + includePath.string() + '\n';
			#endif
			}
		#if API_ID == API_GL
			else
				if(strcmpb(line.c_str(), GL_EXTENSION_DIRECTIVE)) extensions += line;
		#endif
			else
				out += line;
		}

		out += '\n';
	}

	Path GetIncludePath(const std::string& line, const Path& path)
	{
		char delimiter = line.at(9);

		GE_ASSERTM(delimiter == '"' || delimiter == '<', "INVALID INCLUDE DELIMITER!");

		if(delimiter == '<') delimiter = '>';

		const size_t end = line.find(delimiter, 10);

		GE_ASSERTM(end != std::string::npos, "INVALID INCLUDE!");

		if(delimiter == '>')
			return std::filesystem::weakly_canonical(line.substr(10, end - 10));
		return weakly_canonical(path.parent_path() / line.substr(10, end - 10));
	}

	void CompileShaderType(ShaderStageType stage, std::string& out)
	{
		switch(stage)
		{
		case ShaderStageType::Fragment: out += "#define FRAGMENT_SHADER\n"; break;
		case ShaderStageType::Vertex: out += "#define VERTEX_SHADER\n"; break;
		case ShaderStageType::Compute: out += "#define COMPUTE_SHADER\n"; break;
		}
	}
}
