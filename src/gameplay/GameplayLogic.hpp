#pragma once

#include <variant>

#include "core/Action.hpp"
#include "core/EntityId.hpp"
#include "core/GameState.hpp"
#include "gameplay/Events.hpp"
#include "world/Grid.hpp"

struct ActionResult {
    bool changed = false;
    GameState newState;
    std::vector<Event> events;
};


namespace GameplayLogic {
	//bool applyAction(const Grid& grid, GameState& state, Action action);
    ActionResult applyAction(
        const Grid& grid,
        const GameState& state,
        Action action
    );

	bool isWinState(const Grid& grid, const GameState& state);
}