#pragma once

#include <cstddef>

namespace safe_proxy::sdk::offsets {

namespace entity {
inline constexpr std::ptrdiff_t kHealth = 0x100;
inline constexpr std::ptrdiff_t kTeamId = 0x104;
inline constexpr std::ptrdiff_t kDormant = 0x108;
} // namespace entity

} // namespace safe_proxy::sdk::offsets
