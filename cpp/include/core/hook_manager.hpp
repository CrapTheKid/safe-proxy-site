#pragma once

#include <cstddef>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include "core/export.hpp"

namespace safe_proxy::core {

using HookCallback = std::function<void()>;

struct HookRecord {
    std::size_t id{};
    std::string name;
    HookCallback callback;
    bool enabled{true};
};

class SAFE_PROXY_API HookManager final {
public:
    bool initialize();
    void shutdown();

    std::size_t add_hook(std::string name, HookCallback callback);
    bool remove_hook(std::size_t id);
    bool set_enabled(std::size_t id, bool enabled);

    void dispatch();

private:
    std::mutex mutex_;
    std::unordered_map<std::size_t, HookRecord> hooks_;
    std::size_t next_id_{1};
    bool initialized_{false};
};

} // namespace safe_proxy::core
