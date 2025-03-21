//
// Created by scion on 1/17/2025.
//

#pragma once

#include <Graphics/Shader/Shader.h>

namespace gE
{
    class Shader : public Asset
    {
        REFLECTABLE_PROTO("SHDR", "gE::Shader", Shader, Asset);

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

        ~Shader() override = default;

    private:
        Window* _window = DEFAULT;
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
        API::Shader _deferredShader = DEFAULT;
        API::Shader _gBufferShader = DEFAULT;
    };
}
