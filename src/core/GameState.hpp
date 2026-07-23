#pragma once

#include <algorithm>
#include <vector>

#include "core/EntityId.hpp"
#include "world/Player.hpp"
#include "world/Crate.hpp"


struct GameState {
	EntityId nextUnusedEntityId{ 0 };

	Player player{};
	std::vector<Crate> crates{};

	bool operator==(const GameState& other) const {
		if (player.position != other.player.position) {
			return false;
		}
		if (crates.size() != other.crates.size()) {
			return false;
		}
		for (size_t i = 0; i < crates.size(); ++i) {
			if (crates[i].position != other.crates[i].position) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const GameState& other) const {
		return !(*this == other);
	}


	bool isCrateAtPosition(Cell position) const {
		return std::ranges::find(crates, position, &Crate::position) != crates.end();
	}

	Crate* getCrateAtPosition(Cell position)
	{
		auto it = std::ranges::find(crates, position, &Crate::position);
		return it != crates.end() ? &*it : nullptr;
	}

	const Crate* getCrateAtPosition(Cell position) const
	{
		auto it = std::ranges::find(crates, position, &Crate::position);
		return it != crates.end() ? &*it : nullptr;
	}

	EntityId createEntityId()
	{
		return nextUnusedEntityId++;
	}

};
