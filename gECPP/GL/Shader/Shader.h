#pragma once
#include <GL/gl.h>
#include <utility>
#include "GLAD/glad.h"

namespace GL
{
	struct Texture;
	struct ShaderStage;
	typedef std::pair<const char*, const char*> PreprocessorPair;

	enum ShaderStageType : GLenum
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER
	};

	class Shader : public Asset
	{
	 protected:
		Shader(gE::Window*, const char*, PreprocessorPair*, u8);
	 public:
		Shader(gE::Window*, const char* v, const char* f, PreprocessorPair* = nullptr, u8 = 0);
		Shader(gE::Window*, const ShaderStage& v, const ShaderStage& f, PreprocessorPair* = nullptr, u8 = 0);

		inline void Bind() const override { glUseProgram(ID); }

		ALWAYS_INLINE void GetUniform(const char* name) const { glGetUniformLocation(ID, name); }
		ALWAYS_INLINE void SetUniform(u8 loc, u32 val) const { glProgramUniform1ui(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, i32 val) const { glProgramUniform1i(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, float val) const { glProgramUniform1f(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, const gl::vec2& val) const { glProgramUniform2f(ID, loc, val.x, val.y); }
		ALWAYS_INLINE void SetUniform(u8 loc, const gl::vec3& val) const { glProgramUniform3fv(ID, loc, 1, (GLfloat*) &val); }
		ALWAYS_INLINE void SetUniform(u8 loc, const gl::vec4& val) const { glProgramUniform4fv(ID, loc, 1, (GLfloat*) &val); }
		void SetUniform(u8 loc, const Texture*, u8 slot) const;

		inline ~Shader() override { glDeleteProgram(ID); }
	};

	class ComputeShader final : public Shader
	{
		ComputeShader(gE::Window* window, const char* src, PreprocessorPair* pair = nullptr, u8 count = 0) : Shader(window, src, pair, count) {};
		inline void Dispatch(u16 x, u16 y, u16 z) const { Bind(); glDispatchCompute(x, y, z); }
	};

	class ShaderStage final : public Asset
	{
	 public:
		ShaderStage(gE::Window*, ShaderStageType, const char*, PreprocessorPair*, u8);

		inline void Bind() const override {}
		inline void Attach(Shader* s) const { glAttachShader(s->Get(), ID); }

		~ShaderStage() final { glDeleteShader(ID); }
	};
}