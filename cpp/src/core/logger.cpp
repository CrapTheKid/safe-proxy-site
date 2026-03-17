#include "core/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace safe_proxy::core {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::initialize(const std::string& tag) {
    std::scoped_lock lock(mutex_);
    tag_ = tag;
    initialized_ = true;
}

void Logger::shutdown() {
    std::scoped_lock lock(mutex_);
    initialized_ = false;
}

void Logger::write(LogLevel level, const std::string& message) {
    std::scoped_lock lock(mutex_);

    if (!initialized_) {
        initialized_ = true;
    }

    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm{};
#ifdef _WIN32
    localtime_s(&local_tm, &time);
#else
    localtime_r(&time, &local_tm);
#endif

    std::ostringstream stream;
    stream << "[" << tag_ << "] "
           << "[" << std::put_time(&local_tm, "%H:%M:%S") << "] "
           << "[" << level_to_string(level) << "] "
           << message;

    const auto formatted = stream.str();
    std::cout << formatted << std::endl;
    debug_print(formatted);
}

std::string Logger::level_to_string(LogLevel level) const {
    switch (level) {
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Debug:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

void Logger::debug_print(const std::string& text) const {
#ifndef _WIN32
    (void)text;
#else
    std::string buffer = text + "\n";
    OutputDebugStringA(buffer.c_str());
#endif
}

} // namespace safe_proxy::core
