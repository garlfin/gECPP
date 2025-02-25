#pragma once

#include <Core/Serializable/Macro.h>
#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Shader/Preprocessor.h>
#include <Graphics/Shader/Shader.h>

namespace GL
{
	class Texture;
	class Shader;
	class ShaderStage;

	class IShader : public APIObject, public Underlying
	{
		API_DEFAULT_CM_CONSTRUCTOR(IShader);

	public:
		IShader() = default;
		explicit IShader(gE::Window*);

		ALWAYS_INLINE void Bind() const override { glUseProgram(ID); }

		ALWAYS_INLINE u32 GetUniformLocation(const std::string& name) const { return glGetUniformLocation(ID, name.c_str()); }
		std::string GetUniformName(u32 index) const;

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

		inline ~IShader() override { glDeleteProgram(ID); }
	};

	class Shader final : protected GPU::Shader, public IShader
	{
		API_SERIALIZABLE(Shader, GPU::Shader);
		API_DEFAULT_CM_CONSTRUCTOR(Shader);
		API_UNDERLYING_IMPL();
	};

	class ComputeShader final : protected GPU::ComputeShader, public IShader
	{
		API_SERIALIZABLE(ComputeShader, GPU::ComputeShader);
		API_DEFAULT_CM_CONSTRUCTOR(ComputeShader);
		API_UNDERLYING_IMPL();

	 public:
		ALWAYS_INLINE void Dispatch(u16 x, u16 y, u16 z) const { Bind(); glDispatchCompute(x, y, z); }
		ALWAYS_INLINE void Dispatch(u16 x, u16 y) const { Bind(); glDispatchCompute(x, y, 1); }
		ALWAYS_INLINE void Dispatch(u16 x) const { Bind(); glDispatchCompute(x, 1, 1); }

		ALWAYS_INLINE void Dispatch(glm::u16vec3 size) const { Dispatch(size.x, size.y, size.z); }
		ALWAYS_INLINE void Dispatch(glm::u16vec2 size) const { Dispatch(size.x, size.y); }
	};

	class ShaderStage final : protected GPU::ShaderStage, public APIObject
	{
		API_SERIALIZABLE(ShaderStage, GPU::ShaderStage);
		API_DEFAULT_CM_CONSTRUCTOR(ShaderStage);

	 public:
		ALWAYS_INLINE void Bind() const override { }
		ALWAYS_INLINE void Attach(const IShader& shader) const { glAttachShader(shader.Get(), ID); }

		inline ~ShaderStage() override { glDeleteShader(ID); }
	};
}
