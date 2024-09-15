//
// Created by scion on 9/13/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "ShaderStageType.h"

namespace GPU
{
	struct PreprocessorPair
	{
		void Write(std::string& out) const;

		std::string Name;
		std::string Value;
	};

	void CompileDirectives(const Array<PreprocessorPair*>& pairs, std::string& out);
	void CompileIncludes(const char* file, std::string& dst, std::string& directives, std::string& includes);
	void CompileShaderType(ShaderStageType stage, std::string& out);
	const char* GetIncludePath(const char* origin, const char* include);
}

