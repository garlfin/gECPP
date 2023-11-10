//
// Created by scion on 10/25/2023.
//

#pragma once

#include "Shader.h"

namespace GL
{
	class DynamicUniform
	{
	 public:
		DynamicUniform(Shader*, u32);
		DynamicUniform(Shader*, const char*);

		GET_CONST_VALUE(u32, , _location);

		template<class T>
		ALWAYS_INLINE void Set(const T& t) const { if(_location != -1) _shader->SetUniform(_location, t); }

	 private:
		ALWAYS_INLINE u32 GetUniformLocation(const char* name) const
		{
			return glGetUniformLocation(_shader->Get(), name);
		}

		Shader* _shader;
		u32 _location;
	};

	template<>
	void DynamicUniform::Set(const Texture& t) const;

	template<class T>
	class Uniform : private DynamicUniform
	{
	 public:
		using DynamicUniform::DynamicUniform;

		ALWAYS_INLINE void Set(const T& t) const { DynamicUniform::Set(t); }
	};
}