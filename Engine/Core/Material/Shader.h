//
// Created by scion on 1/17/2025.
//

#pragma once

#include <Graphics/Shader/Shader.h>

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

    class Shader : public Asset
    {
        REFLECTABLE_PROTO("SHDR", Shader, Asset);

    public:
        explicit Shader(Window* window) : _window(window) {};

        GET_CONST(Window&, Window, *_window);

        void Bind() const;

        NODISCARD virtual const API::Shader& GetReferenceShader() const = 0;
        NODISCARD virtual const API::Shader& GetShader() const = 0;

#ifdef DEBUG
        NODISCARD virtual bool VerifyUniforms(const std::string&) const = 0;
        NODISCARD bool VerifyUniforms(u32) const;
#endif

        GET_SET(DepthFunction, DepthFunction, _depthFunc);
        GET_SET(CullMode, CullMode, _cullMode);
        GET_SET(BlendMode, BlendMode, _blendMode);

        ~Shader() override = default;

    private:
        Window* _window = DEFAULT;

        DepthFunction _depthFunc = DepthFunction::Less;
        CullMode _cullMode = CullMode::Back;
        BlendMode _blendMode = BlendMode::Dither;
    };

    class ForwardShader final : public Shader
    {
        REFLECTABLE_ONGUI_PROTO(Shader);

    public:
        ForwardShader(Window*, const GPU::Shader& source);

        NODISCARD const API::Shader& GetReferenceShader() const override { return _shader; }
        NODISCARD const API::Shader& GetShader() const override { return _shader; }

        void Free() override { _shader.Free(); }
        NODISCARD bool IsFree() const override { return _shader.IsFree(); }
        void Reload(SETTINGS_T window) override;

#ifdef DEBUG
        NODISCARD bool VerifyUniforms(const std::string&) const override { return true; }
#endif
        ~ForwardShader() override = default;

    private:
        API::Shader _shader = DEFAULT;
    };

    class DeferredShader final : public Shader
    {
    public:
        DeferredShader(Window*, const GPU::Shader& source);

        NODISCARD const API::Shader& GetReferenceShader() const override { return _forwardShader; }
        NODISCARD const API::Shader& GetShader() const override;

        void Free() override;
        NODISCARD bool IsFree() const override;

#ifdef DEBUG
        NODISCARD bool VerifyUniforms(const std::string&) const override;
#endif

        ~DeferredShader() override = default;

    private:
        API::Shader _forwardShader = DEFAULT;
        API::Shader _gBufferShader = DEFAULT;
    };
}
