#include "gameplay/GameplayLogic.hpp"

#include "world/Cell.hpp"

namespace {
	Cell getActionOffset(Action action) {
		switch (action) {
		case Action::MoveUp:
			return { 0, -1 };
		case Action::MoveDown:
			return { 0, 1 };
		case Action::MoveLeft:
			return { -1, 0 };
		case Action::MoveRight:
			return { 1, 0 };
		default:
			return { 0, 0 };
		}
	}


	bool isBlocked(const Grid& grid, const GameState& state, Cell position) {
		return state.isCrateAtPosition(position) || grid.isBlocked(position);
	}

	ActionResult tryMovePlayer(const Grid& grid, const GameState& state, Cell positionDelta) {

		std::vector<Event> events{ NewTurnEvent{} };

		GameState newState = state;

		Cell targetPosition = state.player.position + positionDelta;
		Cell pushPosition = state.player.position + positionDelta * 2;
		auto crate = newState.getCrateAtPosition(targetPosition);
		if (!isBlocked(grid, state, targetPosition)) {
			newState.player.position = targetPosition;
			events.push_back(MoveEvent{ .id = newState.player.id, .type = EntityType::Player, .from = state.player.position, .to = newState.player.position });
			return { true, newState, events };
		}
		else if (crate != nullptr && !isBlocked(grid, state, pushPosition)) {
			crate->position = pushPosition;
			newState.player.position = targetPosition;
			events.push_back(MoveEvent{ .id = newState.player.id, .type = EntityType::Player, .from = state.player.position, .to = newState.player.position });
			events.push_back(MoveEvent{ .id = crate->id, .type = EntityType::Crate, .from = targetPosition, .to = pushPosition });
			return { true, newState, events };
		}
		return {};
	}
} // anonymous namespace

namespace GameplayLogic {
	ActionResult applyAction(const Grid& grid, const GameState& state, Action action) {
		if (action != Action::MoveUp && action != Action::MoveDown && action != Action::MoveLeft && action != Action::MoveRight) {
			return {};
		}
		auto positionDelta = getActionOffset(action);
		if (positionDelta.x == 0 && positionDelta.y == 0) {
			return {}; // Invalid movement action
		}

		return tryMovePlayer(grid, state, positionDelta);

	}

	bool isWinState(const Grid& grid, const GameState& state) {
		for (const Cell& goal : grid.getGoalCells()) {
			if (!state.isCrateAtPosition(goal)) {
				return false;
			}
		}
		return true;
	}

} // namespace GameplayLogic