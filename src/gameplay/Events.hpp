#pragma once

#include <variant>

#include "core/EntityId.hpp"
#include "world/Cell.hpp"

struct MoveEvent {
    EntityId id;
    EntityType type;
    Cell from;
    Cell to;
};

struct CrateDestroyedEvent {
    Cell position;
};

struct LevelWinEvent {
};

struct NewTurnEvent {
};

using Event = std::variant<
    NewTurnEvent,
    MoveEvent,
    CrateDestroyedEvent,
    LevelWinEvent
>;
