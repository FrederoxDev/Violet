#pragma once
#include <cstdint>
#include <functional>

struct DimensionId {
    uint32_t value;

    bool operator==(const DimensionId&) const = default;
    auto operator<=>(const DimensionId&) const = default;
};

namespace std {
    template<>
    struct hash<DimensionId> {
        size_t operator()(const DimensionId& id) const noexcept {
            return std::hash<uint32_t>{}(id.value);
        }
    };
}