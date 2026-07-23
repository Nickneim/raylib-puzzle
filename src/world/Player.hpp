#pragma once

#include "core/EntityId.hpp"
#include "world/Cell.hpp"

struct Player {
    EntityId id;
    Cell position;
};
