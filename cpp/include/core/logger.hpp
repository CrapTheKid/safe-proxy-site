#pragma once

#include <mutex>
#include <string>

#include "core/export.hpp"

namespace safe_proxy::core {

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

class SAFE_PROXY_API Logger final {
public:
    static Logger& instance();

    void initialize(const std::string& tag = "SAFE_PROXY");
    void shutdown();

    void write(LogLevel level, const std::string& message);

private:
    Logger() = default;

    std::string level_to_string(LogLevel level) const;
    void debug_print(const std::string& text) const;

    std::mutex mutex_;
    std::string tag_{"SAFE_PROXY"};
    bool initialized_{false};
};

} // namespace safe_proxy::core

#define SP_LOG_INFO(msg) ::safe_proxy::core::Logger::instance().write(::safe_proxy::core::LogLevel::Info, (msg))
#define SP_LOG_WARN(msg) ::safe_proxy::core::Logger::instance().write(::safe_proxy::core::LogLevel::Warning, (msg))
#define SP_LOG_ERROR(msg) ::safe_proxy::core::Logger::instance().write(::safe_proxy::core::LogLevel::Error, (msg))
#define SP_LOG_DEBUG(msg) ::safe_proxy::core::Logger::instance().write(::safe_proxy::core::LogLevel::Debug, (msg))
