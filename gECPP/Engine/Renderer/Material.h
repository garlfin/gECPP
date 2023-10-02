//
// Created by scion on 9/11/2023.
//

#pragma once
#include "GL/Shader/Shader.h"
#include "Engine/AssetManager.h"

namespace gE
{
	enum class DepthFunction : GLenum
	{
		Disable,
		Less = GL_LESS,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		GreaterEqual = GL_GEQUAL
	};

	enum class BlendMode : GLenum
	{
		Disable,
		Dither,
		Blend
	};

	 struct Material : public GL::Asset
	 {
	 public:
		Material(Window* window, Handle<GL::Shader> shader, DepthFunction depthFunc = DepthFunction::Less);

		void Bind() const final;

		~Material() override = default;

	 private:
		Handle<GL::Shader> const _shader;
		const DepthFunction _depthFunc;
	};
}