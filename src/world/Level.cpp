#include "world/Level.hpp"

#include <iostream>
#include <format>
#include <fstream>

#include "world/Player.hpp"

Level LevelLoader::parseLevelLines(const std::vector<std::string>& lines, const std::string& name, const std::string& author) {
	int gridHeight = static_cast<int>(lines.size());
	if (gridHeight == 0) {
		std::cerr << "Empty level data" << std::endl;
		return Level{};
	}
	int gridWidth = static_cast<int>(lines[0].size());

	std::cout << gridWidth;

	for (const auto& line : lines) {
		std::cout << "line length: " << line.size();
		if (line.size() != gridWidth) {
			std::cerr << "Inconsistent line lengths in level data" << std::endl;
			return Level{};
		}
	}

	Grid grid{ gridWidth, gridHeight };
	GameState state{};

	int y = 0;
	for (const auto& line : lines) {
		int x = 0;
		for (char tileChar : line) {
			Cell position{ x, y };
			switch (tileChar) {
			case '#':
				grid.setTile(position, Tile::Wall);
				break;
			case '.':
				break;
			case 'O':
				grid.setTile(position, Tile::Goal);
				break;
			case 'P':
				state.player = Player{ .id = state.createEntityId(), .position = position };
				break;
			case 'G':
				grid.setTile(position, Tile::Goal);
				state.player = Player{ .id = state.createEntityId(), .position = position };
				break;
			case '*':
				state.crates.push_back(Crate{ .id = state.createEntityId(), .position = position });
				break;
			case '@':
				grid.setTile(position, Tile::Goal);
				state.crates.push_back(Crate{ .id = state.createEntityId(), .position = position });
				break;
			default:
				break;
			}
			x++;
		}
		y++;
	}

	return Level{ .initialState = state, .grid = grid, .name = name, .author = author};
}

std::vector<Level> LevelLoader::loadLevelsFromFile(const std::string& filename) {
	std::vector<Level> levels{};
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open level file: " << filename << std::endl;
		return levels;
	}
	std::string line;
	std::vector<std::string> currentLevelLines{};


	int levelNumber = 1;
	std::string currentLevelName{std::format("Level {}", levelNumber)};
	std::string currentLevelAuthor{};
	while (std::getline(file, line)) {
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}
		if (line.empty()) {
			if (!currentLevelLines.empty()) {
				levels.push_back(parseLevelLines(currentLevelLines, currentLevelName, currentLevelAuthor));
				levelNumber += 1;
				currentLevelName = std::format("Level {}", levelNumber);
				currentLevelAuthor = "";
				currentLevelLines.clear();
			}
		}
		else {
			if (line.starts_with(';')) {
				if (line.starts_with("; Name:")) {
					currentLevelName = line.substr(7);
				}
				else if (line.starts_with("; Author:")) {
					currentLevelAuthor = line.substr(9);
				}
			}
			else {
				currentLevelLines.push_back(line);
			}
		}
	}

	if (!currentLevelLines.empty()) {
		levels.push_back(parseLevelLines(currentLevelLines, currentLevelName, currentLevelAuthor));
	}

	return levels;
}

std::string LevelLoader::serializeLevel(const Level& level)
{
	// We create a grid of characters based on the level's grid and game state, then we join the rows with newlines to create the final string

	int gridWidth = level.grid.getWidth();
	int gridHeight = level.grid.getHeight();

	// We need new lines to separate the rows
	char* serializedLevel = new char[(gridWidth + 1) * gridHeight];

	int x = 0;
	int y = 0;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			auto position{ Cell(x, y) };
			char tileChar = '.';
			if (level.grid.getTile(position) == Tile::Wall) {
				tileChar = '#';
			}
			else if (level.grid.getTile(position) == Tile::Goal) {
				tileChar = 'O';
			}
			if (level.initialState.player.position == position) {
				if (tileChar == 'O') {
					tileChar = 'G';
				}
				else {
					tileChar = 'P';
				}
			}
			else {
				for (const auto& crate : level.initialState.crates) {
					if (crate.position == position) {
						if (tileChar == 'O') {
							tileChar = '@';
						}
						else {
							tileChar = '*';
						}
						break;
					}
				}
			}
			serializedLevel[y * (gridWidth + 1) + x] = tileChar;
		}
		serializedLevel[y * (gridWidth + 1) + gridWidth] = '\n';
	}

	return std::string(serializedLevel);
}

