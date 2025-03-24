//
// Created by scion on 9/11/2023.
//

#pragma once

#include <Core/Pointer.h>
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

	REFLECTABLE_ENUM(Normal, DepthFunction, 5,
		ENUM_PAIR(DepthFunction::Disable, "Disable"),
		ENUM_PAIR(DepthFunction::Less, "Less"),
		ENUM_PAIR(DepthFunction::LessEqual, "Less-Equal"),
		ENUM_PAIR(DepthFunction::Greater, "Greater"),
		ENUM_PAIR(DepthFunction::GreaterEqual, "Greater-Equal")
	);

	enum class CullMode : GLenum
	{
		Disable,
		Back = GL_BACK,
		Front = GL_FRONT,
	};

	REFLECTABLE_ENUM(Normal, CullMode, 3,
		ENUM_PAIR(CullMode::Disable, "Disable"),
		ENUM_PAIR(CullMode::Back, "Back"),
		ENUM_PAIR(CullMode::Front, "Front")
	);

	enum class BlendMode : GLenum
	{
		Disable,
		Dither,
		Blend
	};

	REFLECTABLE_ENUM(Normal, BlendMode, 3,
		ENUM_PAIR(BlendMode::Disable, "Disable"),
		ENUM_PAIR(BlendMode::Dither, "Dither"),
		ENUM_PAIR(BlendMode::Blend, "Blend")
	);

	class Material : public Asset
	{
		REFLECTABLE_PROTO("gE::Material", "MAT", Material, Asset, );

	public:
		Material(Window* window, const Reference<Shader>& shader, DepthFunction depthFunc = DepthFunction::Less, CullMode cullMode = CullMode::Back);

		virtual void Bind() const;

		void Free() override {};
		NODISCARD bool IsFree() const override { return true; }

		GET_CONST(Window&, Window, *_window);
		GET(Shader&, Shader, _shader);

		GET_SET(DepthFunction, DepthFunction, _depthFunc);
		GET_SET(CullMode, CullMode, _cullMode);
		GET_SET(BlendMode, BlendMode, _blendMode);

		~Material() override = default;

	private:
		Window* _window;
		Reference<Shader> _shader;

		DepthFunction _depthFunc = DepthFunction::Less;
		CullMode _cullMode = CullMode::Back;
		BlendMode _blendMode = BlendMode::Dither;
	};
}
