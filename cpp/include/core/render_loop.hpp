#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "core/export.hpp"

namespace safe_proxy::core {

using RenderCallback = std::function<void()>;

class SAFE_PROXY_API RenderLoop final {
public:
    void register_callback(RenderCallback callback);
    void clear_callbacks();

    void on_frame();

private:
    std::mutex mutex_;
    std::vector<RenderCallback> callbacks_;
};

} // namespace safe_proxy::core
