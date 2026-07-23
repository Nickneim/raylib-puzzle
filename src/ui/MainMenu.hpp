#pragma once

#include <string_view>

#include "raylib-cpp.hpp"

#include "core/Action.hpp"
#include "core/SaveData.hpp"
#include "world/Level.hpp"


enum class MainMenuOption {
	StartNewGame,
	Continue,
	LevelSelect,
	ResetLevelProgress,
#if not defined(__EMSCRIPTEN__)
	Exit,
#endif
	Count
};


inline std::string_view mainMenuOptionToString(MainMenuOption option) {
	switch (option) {
	case MainMenuOption::StartNewGame:
		return "Start New Game";
	case MainMenuOption::Continue:
		return "Continue";
	case MainMenuOption::LevelSelect:
		return "Level Select";
	case MainMenuOption::ResetLevelProgress:
		return "Reset Level Progress";
#if not defined(__EMSCRIPTEN__)
	case MainMenuOption::Exit:
		return "Exit";
#endif
	default:
		return "UnknownOptionName";
	}
}

enum class MenuScreen {
	MainMenu,
	LevelSelect
};

inline std::string_view menuScreenToString(MenuScreen screen) {
	switch (screen) {
	case MenuScreen::MainMenu:
		return "MainMenu";
	case MenuScreen::LevelSelect:
		return "LevelSelect";
	default:
		return "UnknownMenuScreen";
	}
}

enum class MenuCommandType {
	None,
	StartNewGame,
	ContinueGame,
	LoadLevel,
	ResetLevelProgress,
	Quit
};

inline std::string_view menuCommandTypeToString(MenuCommandType type) {
	switch (type) {
	case MenuCommandType::None:
		return "None";
	case MenuCommandType::StartNewGame:
		return "StartNewGame";
	case MenuCommandType::ContinueGame:
		return "ContinueGame";
	case MenuCommandType::LoadLevel:
		return "LoadLevel";
	case MenuCommandType::ResetLevelProgress:
		return "ResetLevelProgress";
	case MenuCommandType::Quit:
		return "Quit";
	default:
		return "Unknown MenuCommand";
	}
}

struct MenuCommand {
	MenuCommandType type = MenuCommandType::None;
	int levelIndex = 0;
};


class MainMenu {
public:
	void update(float deltaTime, Action action);
	void draw() const;

	MenuCommand consumePendingCommand() {
		MenuCommand command = pendingCommand;
		pendingCommand = MenuCommand{};
		return command;
	}

	MainMenu(const SaveData& saveData, const std::vector<Level>& levels)
		: saveData(saveData), levels(levels) {}

	MainMenu() = delete;

private:

	static constexpr int MAX_VISIBLE_OPTIONS = 6;

	const SaveData& saveData;
	const std::vector<Level>& levels;
	bool isValidLevelIndex() const;

	int selectedOptionIndex{ 0 };
	MenuScreen activeMenuScreen{ MenuScreen::MainMenu };

	MenuCommand pendingCommand{};

	raylib::Sound gameStartSound{ "resources/sounds/GameStart.wav" };
	raylib::Sound menuMoveSound{ "resources/sounds/MenuMove.wav" };
	raylib::Sound menuSelectSound{ "resources/sounds/MenuSelect.wav" };
};

