//
// Created by scion on 9/11/2023.
//

#pragma once

#include <Utility/AssetManager.h>
#include <Graphics/Shader/Shader.h>

#include "Shader.h"

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

	enum class CullMode : GLenum
	{
		Disable,
		Back = GL_BACK,
		Front = GL_FRONT,
	};

	enum class BlendMode : GLenum
	{
		Disable,
		Dither,
		Blend
	};

	class Material
	{
	 public:
		Material(Window* window, const Reference<Shader>& shader, DepthFunction depthFunc = DepthFunction::Less, CullMode cullMode = CullMode::Back);

		virtual void Bind() const;

		GET_CONST(Window&, Window, *_window);
		GET(Shader&, Shader, _shader);

		virtual ~Material() = default;

	 private:
		Reference<Shader> _shader;
		const DepthFunction _depthFunc;
		const CullMode _cullMode;
		Window* _window;
	};
}
