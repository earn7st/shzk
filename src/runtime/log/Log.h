#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <string_view>

namespace shzk
{
    enum class LogLevel
    {
        Info,
        Warn,
        Error,
    };

    namespace detail
    {
        constexpr std::string_view FileName(std::string_view path)
        {
            auto pos = path.find_last_of("/\\");
            return pos == std::string_view::npos ? path : path.substr(pos + 1);
        }

        template <typename... Args>
        void Log(LogLevel level, std::string_view file, int line,
            fmt::format_string<Args...> fmt, Args&&... args)
        {
            const char* prefix = "";
            switch (level)
            {
            case LogLevel::Info:  prefix = "[SHZK INFO] ";  break;
            case LogLevel::Warn:  prefix = "[SHZK WARN] ";  break;
            case LogLevel::Error: prefix = "[SHZK ERROR] "; break;
            }

            fmt::print("{}[{}:{}] {}\n",
                prefix,
                FileName(file),
                line,
                fmt::format(fmt, std::forward<Args>(args)...));
        }
    }
}

#define SHZK_LOG_INFO(fmt, ...)  \
      ::shzk::detail::Log(::shzk::LogLevel::Info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SHZK_LOG_WARN(fmt, ...)  \
      ::shzk::detail::Log(::shzk::LogLevel::Warn,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define SHZK_LOG_ERROR(fmt, ...) \
      ::shzk::detail::Log(::shzk::LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)