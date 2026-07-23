#pragma once

#include "raylib-cpp.hpp"

#include "world/Cell.hpp"

enum class Tile {
	Floor,
	Wall,
	Goal
};

constexpr int GRID_HORIZONTAL_MARGIN = 40;
constexpr int GRID_VERTICAL_MARGIN = 40;

class Grid {
public:
	Grid(int gridWidth, int gridHeight);
	Tile getTile(Cell cell) const;
	void setTile(Cell cell, Tile tile);
	const std::vector<Cell>& getGoalCells() const;
	bool isBlocked(Cell cell) const;
	void draw() const;

	int getWidth() const { return gridWidth; }
	int getHeight() const { return gridHeight; }
	bool isInsideGrid(Cell cell) const;

	Grid() = default;


private:
	int gridWidth = 0;
	int gridHeight = 0;

	std::vector<Tile> tiles{};
	std::vector<Cell> goalCells{};

};
