//
// Created by scion on 9/13/2024.
//

#pragma once


#include <filesystem>
#include <Graphics/Graphics.h>

#include "ShaderStageType.h"
#include "Serializable/Asset.h"

#define API_INCLUDE_DIRECTIVE "#include "
#define API_DEFINE_DIRECTIVE_LENGTH 10

#define GL_VERSION_DIRECTIVE "#version 460 core\n"
#define GL_EXTENSION_DIRECTIVE "#extension "

namespace GPU
{
	struct PreprocessorPair
	{
		void Write(std::string& out) const;

		std::string Name;
		std::string Value;
	};

	void CompileDirectives(const std::vector<PreprocessorPair>& pairs, std::string& out);
	void CompileIncludes(gE::Window*, std::istream& src, std::string& ext, std::string& out, std::vector<gE::UUID>& inc, const Path& path);
	void CompileShaderType(ShaderStageType stage, std::string& out);
	Path GetIncludePath(const std::string&, const Path& path = DEFAULT);
}

