#include "editor/Editor.hpp"

#include <algorithm>
#include <format>

#include "core/Constants.hpp"
#include "gameplay/GameplayRenderer.hpp"
#include "ui/DebugDrawer.hpp"


void Editor::loadLevel(const Level& newLevel)
{
	level = newLevel;
	GameplayRenderer::updateCamera(camera, level);
}

void Editor::update(float deltaTime, Action action)
{
	if (action == Action::Pause) {
		pendingCommand = { EditorCommandType::OpenMainMenu };
	}

	if (action == Action::LMB) {
		auto [mouseX, mouseY] = camera.GetScreenToWorld(GetMousePosition());
		Cell position{
			static_cast<int>(std::floorf(mouseX / Constants::CellSize)),
			static_cast<int>(std::floorf(mouseY / Constants::CellSize))
		};
		if (level.grid.isInsideGrid(position)) {
			// If the cell is a player, we do nothing
			if (position == level.initialState.player.position) {
				// pass
			}
			else if (activeEditorMode == EditorMode::None) {
				level.grid.setTile(position, Tile::Floor);
				if (level.initialState.isCrateAtPosition(position)) {
					deleteCrateAtPosition(position);
				}
			}
			else if (activeEditorMode == EditorMode::PlaceWalls) {
				if (level.grid.getTile(position) == Tile::Wall) {
					level.grid.setTile(position, Tile::Floor);
				}
				else {
					level.grid.setTile(position, Tile::Wall);
					deleteCrateAtPosition(position);
				}
			}
			else if (activeEditorMode == EditorMode::PlaceGoals) {
				if (level.grid.getTile(position) == Tile::Goal) {
					level.grid.setTile(position, Tile::Floor);
				}
				else {
					level.grid.setTile(position, Tile::Goal);
				}
			}
			else if (activeEditorMode == EditorMode::PlaceCrates) {
				if (level.grid.getTile(position) == Tile::Wall) {
					level.grid.setTile(position, Tile::Floor);
				}
				if (level.initialState.isCrateAtPosition(position)) {
					deleteCrateAtPosition(position);
				}
				else {
					level.initialState.crates.push_back(Crate{ .id = level.initialState.createEntityId(), .position = position});
				}
			}
			else if (activeEditorMode == EditorMode::SetPlayerStart) {
				// We erase whatever's here and place the player
				if (level.grid.getTile(position) == Tile::Wall) {
					level.grid.setTile(position, Tile::Floor);
				}
				if (level.initialState.isCrateAtPosition(position)) {
					deleteCrateAtPosition(position);
				}
				level.initialState.player.position = position;
			}
		}
		else {
			// Clicked outside grid - we interpret this as a desire to resize the grid
			// We resize if the user clicks within 1 cell distance from the current grid bounds, and we set the new bounds to be 1 cell beyond the clicked position (to allow placing tiles on the new edge immediately after resizing)
			if (position.x < -1 || position.y < -1 || position.x > level.grid.getWidth() || position.y > level.grid.getHeight()) {
				// Clicked too far from the grid, do nothing
			}
			else {
				int dx = 0;
				int dy = 0;
				if (position.x == -1) {
					dx = 1;
				}
				if (position.y == -1) {
					dy = 1;
				}

				auto deltaPosition{ Cell(dx, dy) };

				auto newGrid = Grid(level.grid.getWidth() + (position.x == -1 || position.x == level.grid.getWidth() ? 1 : 0),
					level.grid.getHeight() + (position.y == -1 || position.y == level.grid.getHeight() ? 1 : 0));
				// We copy the old grid into the new one possibly moving it if the player clicked to the left or above the current grid
				for (int y = 0; y < level.grid.getHeight(); ++y) {
					for (int x = 0; x < level.grid.getWidth(); ++x) {
						auto cellPosition{ Cell(x, y) };
						newGrid.setTile(cellPosition + deltaPosition, level.grid.getTile(cellPosition));
					}
				}
				level.grid = newGrid;

				level.initialState.player.position += deltaPosition;
				for (auto& crate : level.initialState.crates) {
					crate.position += deltaPosition;
				}

				GameplayRenderer::updateCamera(camera, level);
			}
		}
	}
	else if (action == Action::RMB) {
		activeEditorMode = static_cast<EditorMode>((static_cast<int>(activeEditorMode) + 1) % static_cast<int>(EditorMode::Count));
	}
	else if (action == Action::Editor) {
		pendingCommand = EditorCommand{ EditorCommandType::ReturnToGameplay };
	}
	else if (action == Action::Interact) {
		std::string levelText = LevelLoader::serializeLevel(level);
		raylib::SetClipboardText(levelText);
	}
}



void Editor::draw() const
{
	GameplayRenderer::draw(camera, level, level.initialState, animationSystem);

	auto& debugDrawer = DebugDrawer::get();
	auto mousePosition = GetMousePosition();
	auto worldToScreenPosition = camera.GetWorldToScreen(mousePosition);
	auto screenToWorldPosition = camera.GetScreenToWorld(mousePosition);
	auto [mx, my] = mousePosition;
	auto [wx, wy] = screenToWorldPosition;
	int gridX = static_cast<int>(std::floorf(wx / Constants::CellSize));
	int gridY = static_cast<int>(std::floorf(wy / Constants::CellSize));
	debugDrawer.draw(std::format("GetMousePosition: ({}, {})", mx, my));
	debugDrawer.draw(std::format("GetScreenToWorld: ({}, {})", wx, wy));
	debugDrawer.draw(std::format("GridPosition: ({}, {})", gridX, gridY));

}

void Editor::deleteCrateAtPosition(Cell position)
{
	std::erase_if(level.initialState.crates, [position](const auto& crate) { return crate.position == position; });
}
