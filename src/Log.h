#pragma once

#include "guid.h"

#include <iostream>
#include <vector>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace tiny_rx::utils {

enum class LogSeverity {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

inline std::ostream& operator<<(std::ostream& stream, const Guid& guid) {
    stream << guid.to_string();
    return stream;
}

template <typename Arg, typename... Args>
void log_out(std::ostream& out, Arg&& arg, Args&&... args) {
    out << std::forward<Arg>(arg);
    ((out << ", " << std::forward<Args>(args)), ...);
}

template<typename ...T>
void log(LogSeverity severity, T... args) {
#ifndef LOG_SEVERITY
#ifdef _DEBUG
#define LOG_SEVERITY LogSeverity::Trace
#else
#define LOG_SEVERITY LogSeverity::Warning
#endif
#endif

    if (severity >= LOG_SEVERITY) {
        log_out(std::cout, args...);
        log_out(std::cout, "\n");
    }
}

inline std::string format_func_name(std::string func_name) {
    static constexpr size_t max_func_name_len = 80;
    static constexpr std::string_view ellipsis = "<...>";
    static const std::vector<std::string_view> trim_prefix{ "__thiscall ", "__cdecl " };

    for (const auto& prefix : trim_prefix) {
        if (func_name.substr(0, prefix.size()) == prefix)
            func_name.erase(0, prefix.size());
    }

    if (func_name.size() > max_func_name_len) {
        func_name.resize(max_func_name_len - ellipsis.size());
        func_name += ellipsis;
    }
    return func_name;
}

template<typename ...T>
void trace_call(std::string func_name, T... args) {
    log(LogSeverity::Trace, format_func_name(func_name), args...);
}

} // namespace tiny_rx::utils
