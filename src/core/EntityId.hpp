#pragma once

#include <compare>
#include <functional>

struct EntityId {
    int value = 0;

    constexpr EntityId() = default;
    constexpr explicit EntityId(int value) : value(value) {}

    constexpr EntityId& operator++()
    {
        ++value;
        return *this;
    }

    constexpr EntityId operator++(int)
    {
        EntityId copy = *this;
        ++(*this);
        return copy;
    }

    auto operator<=>(const EntityId&) const = default;
};


template<>
struct std::hash<EntityId>
{
    std::size_t operator()(const EntityId& id) const noexcept
    {
        return std::hash<int>{}(id.value);
    }
};


enum class EntityType {
    Player,
    Crate
};