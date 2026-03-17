#pragma once

#include <atomic>

#include "core/export.hpp"
#include "core/feature_manager.hpp"
#include "core/hook_manager.hpp"
#include "core/render_loop.hpp"

namespace safe_proxy::core {

class SAFE_PROXY_API Project final {
public:
    static Project& instance();

    bool initialize();
    void shutdown();

    void on_frame();

    HookManager& hook_manager() noexcept { return hook_manager_; }
    RenderLoop& render_loop() noexcept { return render_loop_; }
    FeatureManager& feature_manager() noexcept { return feature_manager_; }

private:
    Project() = default;

    std::atomic<bool> running_{false};
    HookManager hook_manager_;
    RenderLoop render_loop_;
    FeatureManager feature_manager_;
};

} // namespace safe_proxy::core
