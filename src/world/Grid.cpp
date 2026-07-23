#include "world/Grid.hpp"

#include "core/Constants.hpp"

Grid::Grid(int gridWidth, int gridHeight)
	: gridWidth(gridWidth),
	gridHeight(gridHeight),
	tiles(std::vector<Tile>(gridWidth * gridHeight, Tile::Floor))
{
}

Tile Grid::getTile(Cell cell) const {
	if (!isInsideGrid(cell)) {
		return Tile::Wall;
	}
	return tiles[cell.y * gridWidth + cell.x];
}

void Grid::setTile(Cell cell, Tile tile)
{
	if (!isInsideGrid(cell)) {
		return;
	}

	bool wasGoal = getTile(cell) == Tile::Goal;

	if (!wasGoal && tile == Tile::Goal) {
		goalCells.push_back(cell);
	}
	else if (wasGoal && tile != Tile::Goal) {
		std::erase(goalCells, cell);
	}

	tiles[cell.y * gridWidth + cell.x] = tile;
}

const std::vector<Cell>& Grid::getGoalCells() const
{
	return goalCells;
}

bool Grid::isInsideGrid(Cell cell) const {
	return cell.x >= 0 && cell.x < gridWidth && cell.y >= 0 && cell.y < gridHeight;
}

bool Grid::isBlocked(Cell cell) const {
	return getTile(cell) == Tile::Wall;
}
//
//int Grid::calculateCellSize() const {
//	int cellSizeX = (raylib::Window::GetWidth() - 2 * GRID_HORIZONTAL_MARGIN) / std::max(gridWidth, 1);
//	int cellSizeY = (raylib::Window::GetHeight() - 2 * GRID_VERTICAL_MARGIN) / std::max(gridHeight, 1);
//	return std::min(cellSizeX, cellSizeY);
//}
//
//raylib::Vector2 Grid::calculateGridOrigin() const {
//	int gridWidthPixels = gridWidth * cellSize;
//	int gridHeightPixels = gridHeight * cellSize;
//	float offsetX = (raylib::Window::GetWidth() - gridWidthPixels) / 2.0f;
//	float offsetY = (raylib::Window::GetHeight() - gridHeightPixels) / 2.0f;
//	return raylib::Vector2{ offsetX, offsetY };
//}
//

void Grid::draw() const {
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {
			raylib::Rectangle rectangle {
				static_cast<float>(x * Constants::CellSize),
				static_cast<float>(y * Constants::CellSize),
				Constants::CellSize,
				Constants::CellSize
			};

			rectangle.DrawLines(raylib::Color::Black());

			if (getTile({x, y}) == Tile::Wall) {
				rectangle.Draw(raylib::Color::Gray());
			}
			else if (getTile({ x, y }) == Tile::Goal) {
				rectangle.Draw(raylib::Color::Yellow());
			}
		}
	}
}
