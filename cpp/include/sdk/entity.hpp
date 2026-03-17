#pragma once

#include <cstdint>

#include "core/memory.hpp"
#include "sdk/offsets.hpp"

namespace safe_proxy::sdk {

class Entity {
public:
    explicit Entity(std::uintptr_t base_address) : base_address_(base_address) {}

    [[nodiscard]] std::uintptr_t base_address() const noexcept { return base_address_; }

    [[nodiscard]] int health() const;
    [[nodiscard]] int team_id() const;
    [[nodiscard]] bool is_dormant() const;

private:
    std::uintptr_t base_address_{};
};

} // namespace safe_proxy::sdk
