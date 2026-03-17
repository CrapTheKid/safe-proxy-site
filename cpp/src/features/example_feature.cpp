#include "features/example_feature.hpp"

#include "core/logger.hpp"

namespace safe_proxy::features {

void ExampleFeature::on_enable() {
    SP_LOG_INFO("ExampleFeature enabled");
}

void ExampleFeature::on_disable() {
    SP_LOG_INFO("ExampleFeature disabled");
}

void ExampleFeature::on_frame() {
    SP_LOG_DEBUG("ExampleFeature tick");
}

} // namespace safe_proxy::features
