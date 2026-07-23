#pragma once

#include <string>
#include <vector>

#include "core/GameState.hpp"
#include "world/Grid.hpp"


struct Level {
	GameState initialState;
	Grid grid;
	std::string name{};
	std::string author{};
};

namespace LevelLoader {
	Level parseLevelLines(const std::vector<std::string>& lines, const std::string& name, const std::string& author);

	std::vector<Level> loadLevelsFromFile(const std::string& filename);
	std::string serializeLevel(const Level& level);
}
