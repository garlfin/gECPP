//
// Created by scion on 9/11/2023.
//

#pragma once

#include <Graphics/Shader/Uniform.h>
#include <Graphics/Texture/Texture.h>
#include <Engine/AssetManager.h>

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

	struct Material : public API::APIObject
	{
	 public:
		Material(Window* window, const Reference<API::Shader>& shader, DepthFunction depthFunc = DepthFunction::Less, CullMode cullMode = CullMode::Back);

		void Bind() const override;

		GET_CONST(API::Shader &, Shader, _shader);

	 private:
		const Reference<API::Shader> _shader;
		const DepthFunction _depthFunc;
		const CullMode _cullMode;
	};

	template<class T>
	class ValueUniform : private API::Uniform<std::remove_const_t<std::remove_reference_t<T>>>
	{
	 public:
		typedef std::remove_const_t<std::remove_reference_t<T>> I;

		ValueUniform(const Material* mat, const char* n, const I& t) : API::Uniform<I>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, const char* n, I&& t) : API::Uniform<I>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, u32 l, const I& t) : API::Uniform<I>(&mat->GetShader(), l), _t(t) { };
		ValueUniform(const Material* mat, u32 l, I&& t) : API::Uniform<I>(&mat->GetShader(), l), _t(t) { };

		ValueUniform(const ValueUniform&) = default;
		ValueUniform(ValueUniform&&) = default;

		ALWAYS_INLINE ValueUniform& operator=(const I& t){ _t = t; return *this; }
		ALWAYS_INLINE ValueUniform& operator=(I&& t) noexcept { _t = t; return *this; }

		ALWAYS_INLINE ValueUniform& operator=(ValueUniform&&) noexcept = default;
		ALWAYS_INLINE ValueUniform& operator=(const ValueUniform&) = default;
		ALWAYS_INLINE I* operator->() const { return _t; }
		ALWAYS_INLINE I& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator I*() const { return _t; } // NOLINT
		ALWAYS_INLINE operator I&() const { return *_t; } // NOLINT

		ALWAYS_INLINE void Set() const { API::Uniform<I>::Set(_t); }

		// I just prefer the semantics of it being encapsulated
		GET_SET(I&, , _t);
		SET_XVAL(I, , _t);

	 private:
		T _t;
	};

	template<class T>
	using ReferenceUniform = ValueUniform<Reference<T>>;

	template<class T>
	using SmartPointerUniform = ValueUniform<SmartPointer<T>>;
}