#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "core/export.hpp"
#include "features/feature.hpp"

namespace safe_proxy::core {

class SAFE_PROXY_API FeatureManager final {
public:
    bool register_feature(std::shared_ptr<features::IFeature> feature);
    bool unregister_feature(const std::string& name);

    bool enable_feature(const std::string& name);
    bool disable_feature(const std::string& name);

    void on_frame();
    void clear();

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<features::IFeature>> features_;
};

} // namespace safe_proxy::core
