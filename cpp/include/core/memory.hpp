#pragma once

#include <cstdint>
#include <cstring>
#include <optional>
#include <type_traits>

namespace safe_proxy::core {

class Memory {
public:
    template <typename T>
    static std::optional<T> read(std::uintptr_t address) {
        static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable.");
        if (address == 0U) {
            return std::nullopt;
        }

        T value{};
        std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(T));
        return value;
    }

    template <typename T>
    static bool write(std::uintptr_t address, const T& value) {
        static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable.");
        if (address == 0U) {
            return false;
        }

        std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(T));
        return true;
    }

    template <typename T>
    static T* as_ptr(std::uintptr_t address) {
        return reinterpret_cast<T*>(address);
    }

    template <typename T>
    static const T* as_ptr_const(std::uintptr_t address) {
        return reinterpret_cast<const T*>(address);
    }

    template <typename T>
    static std::uintptr_t offset(std::uintptr_t base, std::ptrdiff_t offset) {
        return static_cast<std::uintptr_t>(base + static_cast<std::uintptr_t>(offset));
    }
};

} // namespace safe_proxy::core
