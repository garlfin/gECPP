//
// Created by scion on 9/11/2023.
//

#pragma once

#include <GL/Shader/Shader.h>
#include <GL/Shader/Uniform.h>
#include <GL/Texture/Texture.h>
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

	struct Material : public GL::Asset
	{
	 public:
		Material(Window* window, const Reference<GL::Shader>& shader, DepthFunction depthFunc = DepthFunction::Less, CullMode cullMode = CullMode::Back);

		void Bind() const override;

		GET_CONST(GL::Shader &, Shader, _shader);

	 private:
		const Reference<GL::Shader> _shader;
		const DepthFunction _depthFunc;
		const CullMode _cullMode;
	};

	template<class T>
	class ValueUniform : private GL::Uniform<T>
	{
	 public:
		ValueUniform(const Material* mat, const char* n, const T& t) : GL::Uniform<T>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, const char* n, T&& t) : GL::Uniform<T>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, u32 l, const T& t) : GL::Uniform<T>(&mat->GetShader(), l), _t(t) { };
		ValueUniform(const Material* mat, u32 l, T&& t) : GL::Uniform<T>(&mat->GetShader(), l), _t(t) { };

		ValueUniform(const ValueUniform&) = default;
		ValueUniform(ValueUniform&&) = default;

		ALWAYS_INLINE ValueUniform& operator=(const T& t){ _t = t; return *this; }
		ALWAYS_INLINE ValueUniform& operator=(T&& t) noexcept { _t = t; return *this; }

		ALWAYS_INLINE ValueUniform& operator=(ValueUniform&&) noexcept = default;
		ALWAYS_INLINE ValueUniform& operator=(const ValueUniform&) = default;
		ALWAYS_INLINE T* operator->() const { return _t; }
		ALWAYS_INLINE T& operator*() const { return *_t; }
		ALWAYS_INLINE operator bool() const { return (bool) _t; } // NOLINT
		ALWAYS_INLINE operator T*() const { return _t; } // NOLINT
		ALWAYS_INLINE operator T&() const { return *_t; } // NOLINT

		ALWAYS_INLINE void Set() const { GL::Uniform<T>::Set(_t); }

		// I just prefer the semantics of it being encapsulated
		GET_SET(T&, , _t);
		SET_XVAL(T, , _t);

	 private:
		T _t;
	};

	template<class T>
	using ReferenceUniform = ValueUniform<gE::Reference<T>>;

	template<class T>
	using SmartPointerUniform = ValueUniform<gE::SmartPointer<T>>;
}