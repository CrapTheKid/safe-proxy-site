#include "core/feature_manager.hpp"

#include "core/logger.hpp"

namespace safe_proxy::core {

bool FeatureManager::register_feature(std::shared_ptr<features::IFeature> feature) {
    if (!feature) {
        return false;
    }

    std::scoped_lock lock(mutex_);
    return features_.emplace(feature->name(), std::move(feature)).second;
}

bool FeatureManager::unregister_feature(const std::string& name) {
    std::scoped_lock lock(mutex_);
    return features_.erase(name) > 0;
}

bool FeatureManager::enable_feature(const std::string& name) {
    std::scoped_lock lock(mutex_);
    const auto it = features_.find(name);
    if (it == features_.end()) {
        return false;
    }

    if (!it->second->enabled()) {
        it->second->set_enabled(true);
        it->second->on_enable();
        SP_LOG_INFO("Enabled feature: " + name);
    }

    return true;
}

bool FeatureManager::disable_feature(const std::string& name) {
    std::scoped_lock lock(mutex_);
    const auto it = features_.find(name);
    if (it == features_.end()) {
        return false;
    }

    if (it->second->enabled()) {
        it->second->on_disable();
        it->second->set_enabled(false);
        SP_LOG_INFO("Disabled feature: " + name);
    }

    return true;
}

void FeatureManager::on_frame() {
    std::scoped_lock lock(mutex_);
    for (const auto& [name, feature] : features_) {
        (void)name;
        if (feature && feature->enabled()) {
            feature->on_frame();
        }
    }
}

void FeatureManager::clear() {
    std::scoped_lock lock(mutex_);
    features_.clear();
}

} // namespace safe_proxy::core
