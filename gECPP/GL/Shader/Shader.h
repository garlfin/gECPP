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

		PreprocessorPair(PreprocessorPair&& o) noexcept : Name(o.Name), Value(o.Value)
		{
			o.Name = nullptr;
		}

		PreprocessorPair(const PreprocessorPair& o);
		PreprocessorPair() = default;

		OPERATOR_EQUALS_BOTH(PreprocessorPair);

		char* Name = nullptr;
		char* Value = nullptr;

		void Write(std::string&) const;

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
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::vec3& val) const { glProgramUniform3f(ID, loc, val.x, val.y, val.z); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::vec4& val) const { glProgramUniform4f(ID, loc, val.x, val.y, val.z, val.w); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::ivec2& val) const { glProgramUniform2i(ID, loc, val.x, val.y); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::ivec3& val) const { glProgramUniform3i(ID, loc, val.x, val.y, val.z); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::ivec4& val) const { glProgramUniform4i(ID, loc, val.x, val.y, val.z, val.w); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::uvec2& val) const { glProgramUniform2ui(ID, loc, val.x, val.y); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::uvec3& val) const { glProgramUniform3ui(ID, loc, val.x, val.y, val.z); }
		ALWAYS_INLINE void SetUniform(u8 loc, const glm::uvec4& val) const { glProgramUniform4ui(ID, loc, val.x, val.y, val.z, val.w); }

		void SetUniform(u8 loc, const Texture&, u8 slot) const;
		void SetUniform(u8 loc, const Texture&) const;

		~Shader() override { glDeleteProgram(ID); }
	};

	class ComputeShader final : public Shader
	{
	 public:
		ComputeShader(gE::Window* window, const char* src, const Array<PreprocessorPair>* pair = nullptr)
			: Shader(window, src, pair)
		{ };

		ALWAYS_INLINE void Dispatch(u16 x, u16 y, u16 z) const { glDispatchCompute(x, y, z); }
		ALWAYS_INLINE void Dispatch(u16 x, u16 y) const { glDispatchCompute(x, y, 1); }
		ALWAYS_INLINE void Dispatch(u16 x) const { glDispatchCompute(x, 1, 1); }

		ALWAYS_INLINE void Dispatch(glm::u16vec3 s) const { Dispatch(s.x, s.y, s.z); }
		ALWAYS_INLINE void Dispatch(glm::u16vec2 s) const { Dispatch(s.x, s.y); }
	};

	class ShaderStage final : public Asset
	{
	 public:
		ShaderStage(gE::Window*, ShaderStageType, const char*, const Array<PreprocessorPair>*);

		inline void Bind() const override { }

		ALWAYS_INLINE void Attach(Shader* s) const { glAttachShader(s->Get(), ID); }

		~ShaderStage() final { glDeleteShader(ID); }
	};
}