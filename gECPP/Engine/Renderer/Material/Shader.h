//
// Created by scion on 1/17/2025.
//

#pragma once

#include <Graphics/Shader/Shader.h>

namespace gE
{
    class Shader
    {
    public:
        explicit Shader(Window& window) : _window(&window) {};

        GET_CONST(Window&, Window, *_window);

        void Bind() const { GetShader().Bind(); }

        NODISCARD virtual const API::Shader& GetReferenceShader() const = 0;
        NODISCARD virtual const API::Shader& GetShader() const = 0;
        NODISCARD virtual Array<const API::Shader*> GetAllShaders() const = 0;

        virtual ~Shader() = default;

    private:
        Window* _window = DEFAULT;
    };

    class ForwardShader final : public Shader
    {
    public:
        ForwardShader(Window&, const GPU::Shader& source);

        NODISCARD const API::Shader& GetReferenceShader() const override { return _shader; }
        NODISCARD const API::Shader& GetShader() const override { return _shader; };
        NODISCARD Array<const API::Shader*> GetAllShaders() const override { return {  &_shader }; }

        ~ForwardShader() override = default;

    private:
        API::Shader _shader = DEFAULT;
    };

    class DeferredShader final : public Shader
    {
    public:
        DeferredShader(Window&, const GPU::Shader& source);

        NODISCARD const API::Shader& GetReferenceShader() const override { return _forwardShader; }
        NODISCARD const API::Shader& GetShader() const override;
        NODISCARD Array<const API::Shader*> GetAllShaders() const override;

        ~DeferredShader() override = default;

    private:
        API::Shader _forwardShader = DEFAULT;
        API::Shader _deferredShader = DEFAULT;
        API::Shader _gBufferShader = DEFAULT;
    };
}
