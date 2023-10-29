#pragma once
#include <GL/GL.h>
#include <utility>
#include "GLAD/glad.h"
#include "Engine/Array.h"

namespace GL
{
	struct ShaderStage;
	struct Texture;

	struct PreprocessorPair
	{
		explicit PreprocessorPair(const char* n, const char* v = nullptr);
		PreprocessorPair(PreprocessorPair&& o) noexcept : Name(o.Name), Value(o.Value) { o.Name = nullptr; }
		PreprocessorPair(const PreprocessorPair& o);
		PreprocessorPair() = default;

		OPERATOR_EQUALS_BOTH(PreprocessorPair);

		char* Name = nullptr;
		char* Value = nullptr;

		void WriteDirective(gETF::SerializationBuffer&) const;

		~PreprocessorPair() { delete[] Name; }
	};

	enum ShaderStageType : GLenum
	{
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER,
		Compute = GL_COMPUTE_SHADER
	};

	const char* ShaderStageDefine(ShaderStageType type);

	class Shader : public Asset
	{
	 protected:
		Shader(gE::Window*, const char*, const Array<PreprocessorPair>*);
	 public:
		Shader(gE::Window*, const char* v, const char* f, const Array<PreprocessorPair>* = nullptr);
		Shader(gE::Window*, const ShaderStage& v, const ShaderStage& f);

		ALWAYS_INLINE void Bind() const override { glUseProgram(ID); }

		ALWAYS_INLINE void GetUniform(const char* name) const { glGetUniformLocation(ID, name); }
		ALWAYS_INLINE void SetUniform(u8 loc, u32 val) const { glProgramUniform1ui(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, i32 val) const { glProgramUniform1i(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, float val) const { glProgramUniform1f(ID, loc, val); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::vec2& val) const { glProgramUniform2f(ID, loc, val.x, val.y); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::vec3& val) const { glProgramUniform3fv(ID, loc, 1, (GLfloat*) &val); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::vec4& val) const { glProgramUniform4fv(ID, loc, 1, (GLfloat*) &val); }
		void SetUniform(u8 loc, const Texture&, u8 slot) const;
		void SetUniform(u8 loc, const Texture&) const;

		~Shader() override { glDeleteProgram(ID); }
	};

	class ComputeShader final : public Shader
	{
		ComputeShader(gE::Window* window, const char* src, const Array<PreprocessorPair>* pair) : Shader(window, src, pair) {};
		ALWAYS_INLINE void Dispatch(u16 x, u16 y, u16 z) const { Bind(); glDispatchCompute(x, y, z); }
	};

	class ShaderStage final : public Asset
	{
	 public:
		ShaderStage(gE::Window*, ShaderStageType, const char*, const Array<PreprocessorPair>*);

		inline void Bind() const override {}
		ALWAYS_INLINE void Attach(Shader* s) const { glAttachShader(s->Get(), ID); }

		~ShaderStage() final { glDeleteShader(ID); }
	};
}