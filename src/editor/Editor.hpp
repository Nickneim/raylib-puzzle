#pragma once

#include "core/Action.hpp"
#include "systems/AnimationSystem.hpp"
#include "world/Level.hpp"

enum class EditorMode {
	None,
	PlaceWalls,
	PlaceGoals,
	PlaceCrates,
	SetPlayerStart,
	Count
};

inline std::string_view editorModeToString(EditorMode mode) {
	switch (mode) {
	case EditorMode::None:
		return "None";
	case EditorMode::PlaceWalls:
		return "PlaceWalls";
	case EditorMode::PlaceGoals:
		return "PlaceGoals";
	case EditorMode::PlaceCrates:
		return "PlaceCrates";
	case EditorMode::SetPlayerStart:
		return "SetPlayerStart";
	default:
		return "UnknownEditorMode";
	}
}

enum class EditorCommandType {
	None,
	OpenMainMenu,
	ReturnToGameplay
};

struct EditorCommand {
	EditorCommandType type{ EditorCommandType::None };
};

class Editor {
public:
	void loadLevel(const Level& newLevel);
	void update(float deltaTime, Action action);
	void draw() const;

	Level level{};

	EditorCommand consumePendingCommand() {
		EditorCommand command = pendingCommand;
		pendingCommand = EditorCommand{};
		return command;
	}

private:
	EditorMode activeEditorMode{ EditorMode::None };

	AnimationSystem animationSystem{};
	raylib::Camera2D camera{};

	EditorCommand pendingCommand{};

	void deleteCrateAtPosition(Cell position);
};
