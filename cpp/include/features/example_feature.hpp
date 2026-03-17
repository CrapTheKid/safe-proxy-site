#pragma once

#include "features/feature.hpp"

namespace safe_proxy::features {

class ExampleFeature final : public IFeature {
public:
    std::string name() const override { return "ExampleFeature"; }

    void on_enable() override;
    void on_disable() override;
    void on_frame() override;
};

} // namespace safe_proxy::features
