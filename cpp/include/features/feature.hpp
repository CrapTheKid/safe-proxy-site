#pragma once

#include <string>

namespace safe_proxy::features {

class IFeature {
public:
    virtual ~IFeature() = default;

    virtual std::string name() const = 0;
    virtual void on_enable() = 0;
    virtual void on_disable() = 0;
    virtual void on_frame() = 0;

    bool enabled() const noexcept { return enabled_; }
    void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

private:
    bool enabled_{false};
};

} // namespace safe_proxy::features
