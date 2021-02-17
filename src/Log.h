#pragma once

#include <iostream>

namespace tirx::utils {

enum class LogSeverity {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

template <typename Arg, typename... Args>
void log_out(std::ostream& out, Arg&& arg, Args&&... args) {
    out << std::forward<Arg>(arg);
    ((out << ',' << std::forward<Args>(args)), ...);
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
        log_out(std::cout, args..., "\n");
    }
}

}
