#include "core/hook_manager.hpp"

#include "core/logger.hpp"

namespace safe_proxy::core {

bool HookManager::initialize() {
    std::scoped_lock lock(mutex_);
    initialized_ = true;
    SP_LOG_INFO("HookManager initialized");
    return true;
}

void HookManager::shutdown() {
    std::scoped_lock lock(mutex_);
    hooks_.clear();
    initialized_ = false;
    SP_LOG_INFO("HookManager shutdown");
}

std::size_t HookManager::add_hook(std::string name, HookCallback callback) {
    std::scoped_lock lock(mutex_);
    if (!initialized_) {
        return 0;
    }

    const auto id = next_id_++;
    hooks_.emplace(id, HookRecord{ id, std::move(name), std::move(callback), true });
    return id;
}

bool HookManager::remove_hook(std::size_t id) {
    std::scoped_lock lock(mutex_);
    return hooks_.erase(id) > 0;
}

bool HookManager::set_enabled(std::size_t id, bool enabled) {
    std::scoped_lock lock(mutex_);
    const auto it = hooks_.find(id);
    if (it == hooks_.end()) {
        return false;
    }

    it->second.enabled = enabled;
    return true;
}

void HookManager::dispatch() {
    std::unordered_map<std::size_t, HookRecord> copy;
    {
        std::scoped_lock lock(mutex_);
        if (!initialized_) {
            return;
        }
        copy = hooks_;
    }

    for (auto& [_, record] : copy) {
        if (record.enabled && record.callback) {
            record.callback();
        }
    }
}

} // namespace safe_proxy::core
