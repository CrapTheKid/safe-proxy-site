#include "core/render_loop.hpp"

namespace safe_proxy::core {

void RenderLoop::register_callback(RenderCallback callback) {
    std::scoped_lock lock(mutex_);
    callbacks_.push_back(std::move(callback));
}

void RenderLoop::clear_callbacks() {
    std::scoped_lock lock(mutex_);
    callbacks_.clear();
}

void RenderLoop::on_frame() {
    std::vector<RenderCallback> copy;
    {
        std::scoped_lock lock(mutex_);
        copy = callbacks_;
    }

    for (const auto& callback : copy) {
        if (callback) {
            callback();
        }
    }
}

} // namespace safe_proxy::core
