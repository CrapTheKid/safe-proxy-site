#include "sdk/entity.hpp"

namespace safe_proxy::sdk {

int Entity::health() const {
    const auto address = core::Memory::offset<int>(base_address_, offsets::entity::kHealth);
    const auto value = core::Memory::read<int>(address);
    return value.value_or(0);
}

int Entity::team_id() const {
    const auto address = core::Memory::offset<int>(base_address_, offsets::entity::kTeamId);
    const auto value = core::Memory::read<int>(address);
    return value.value_or(0);
}

bool Entity::is_dormant() const {
    const auto address = core::Memory::offset<bool>(base_address_, offsets::entity::kDormant);
    const auto value = core::Memory::read<bool>(address);
    return value.value_or(true);
}

} // namespace safe_proxy::sdk
