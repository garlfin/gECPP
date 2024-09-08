//
// Created by scion on 9/11/2023.
//

#pragma once

#include <Graphics/Shader/Shader.h>
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

	class Material
	{
	 public:
		Material(Window* window, const Reference<API::Shader>& shader, DepthFunction depthFunc = DepthFunction::Less, CullMode cullMode = CullMode::Back);

		virtual void Bind() const;

		GET_CONST(API::Shader &, Shader, _shader);
		GET_CONST(Window&, Window, *_window);

		virtual ~Material() = default;

	 private:
		const Reference<API::Shader> _shader;
		const DepthFunction _depthFunc;
		const CullMode _cullMode;
		Window* _window;
	};

	template<class T>
	class ValueUniform : private API::Uniform<T>
	{
	 public:
		ValueUniform(const Material* mat, const char* n, const T& t) : API::Uniform<T>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, const char* n, T&& t) : API::Uniform<T>(&mat->GetShader(), n), _t(t) { };
		ValueUniform(const Material* mat, u32 l, const T& t) : API::Uniform<T>(&mat->GetShader(), l), _t(t) { };
		ValueUniform(const Material* mat, u32 l, T&& t) : API::Uniform<T>(&mat->GetShader(), l), _t(t) { };

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

		ALWAYS_INLINE void Set() const { API::Uniform<T>::Set(_t); }

		GET_SET(T&, , _t);
		SET_XVAL(T, , _t);

	 private:
		T _t;
	};

	template<class T>
	using ReferenceUniform = ValueUniform<Reference<T>>;

	template<class T>
	using SmartPointerUniform = ValueUniform<SmartPointer<T>>;
}