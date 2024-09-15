//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "Shader.h"

namespace GPU
{
	class DynamicUniform
	{
	public:
		DynamicUniform(API::Shader* shader, u32 location);
		DynamicUniform(API::Shader* shader, const std::string& name);

		GET_CONST(const API::Shader&, Shader, *_shader);
		GET_CONST(u32, , _location);

		template<class T>
		ALWAYS_INLINE void Set(const T& t) const { if(_location != -1) _shader->SetUniform(_location, t); }

	private:
		API::Shader* _shader;
		u32 _location;
	};

	template<>
	void DynamicUniform::Set(const API::Texture& t) const;

	template<class T>
	class Uniform : private DynamicUniform
	{
	public:
		using DynamicUniform::DynamicUniform;

		using DynamicUniform::Get;
		using DynamicUniform::GetShader;

		ALWAYS_INLINE void Set(const T& t) const { DynamicUniform::Set(t); }
	};
}