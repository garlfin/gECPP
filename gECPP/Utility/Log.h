//
// Created by scion on 2/6/2025.
//

#pragma once

#include <string>
#include <format>

#include "Macro.h"

namespace gE
{
    class Log
    {
    public:
        template<class... ARGS>
        static void Write(const std::string_view& format, ARGS&&... args)
        {
            if constexpr(sizeof...(ARGS) == 0)
                _log += format;
            else
                _log += std::vformat(format, std::make_format_args(args...));
        }

        template<class... ARGS>
        static void WriteLine(const std::string_view& format, ARGS&&... args)
        {
            if constexpr(sizeof...(ARGS) == 0)
                _log += format;
            else
                _log += std::vformat(format, std::make_format_args(args...));
            _log += '\n';
        }

        static void FatalError(const std::string_view& message);

        template<class... ARGS>
        static void FatalError(const std::string_view& format, ARGS&&... args)
        {
            FatalError(std::vformat(format, std::make_format_args(args...)));
        }

        NODISCARD ALWAYS_INLINE static std::string_view Get() { return _log; };

        inline static void Clear() { _log.clear(); _log.shrink_to_fit(); }

    private:
        Log() = default;

        inline static std::string _log = DEFAULT;
    };
}
