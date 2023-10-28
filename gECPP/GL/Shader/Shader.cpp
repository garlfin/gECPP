//
// Created by scion on 8/24/2023.
//

#include "Shader.h"
#include "Uniform.h"
#include <GL/Texture/Texture.h>
#include <iostream>
#include <Engine/Window.h>

#define VERSION_DIRECTIVE "#version 460 core\n"

#define GL_GET(FUNC_STAGE, FUNC_SHADER, ARGS...) if constexpr (std::is_same_v<T, ShaderStage>) FUNC_STAGE(ARGS); else FUNC_SHADER(ARGS);

namespace GL
{
	void CompileDirectives(const Array<PreprocessorPair>*, gETF::SerializationBuffer&);
	void CompileIncludes(const char* file, gETF::SerializationBuffer&, gETF::SerializationBuffer&, gETF::SerializationBuffer& idBuffer);
	const char* GetIncludePath(const char* origin, const char* include);

	template<typename T>
	bool GetShaderStatus(const T& shader, char* source = nullptr);

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
					else std::cout << "0(0): " << *source;
					i++;
				}
				else std::cout << *source;
		}
		std::cout << '\n';
		#endif

		delete[] infoLog;
		return false;
	}

	void Shader::SetUniform(u8 loc, const Texture& tex, u8 slot) const
	{
		SetUniform(loc, tex.Use(slot));
	}

	DynamicUniform::DynamicUniform(Shader* s, u32 l) : _shader(s), _location(l) { }
	DynamicUniform::DynamicUniform(Shader* s, const char* n) : _shader(s), _location(GetUniformLocation(n)) { }

	template<>
	void DynamicUniform::Set(const Texture& t) const
	{
		_shader->SetUniform(_location, t, _shader->GetWindow()->GetSlotManager().Increment(&t));
	}
}

