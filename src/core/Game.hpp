// Game.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <array>
#include <iostream>
#include <stack>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

#include "raylib-cpp.hpp"

#include "core/Action.hpp"
#include "core/GameState.hpp"
#include "editor/Editor.hpp"
#include "gameplay/GameplayController.hpp"
#include "systems/InputManager.hpp"
#include "ui/MainMenu.hpp"
#include "world/Level.hpp"
#include "core/SaveData.hpp"

enum class GameMode {
	Gameplay,
	MainMenu,
	Editor
};


inline std::string_view gameModeToString(GameMode mode) {
	switch (mode) {
	case GameMode::Gameplay:
		return "Gameplay";
	case GameMode::MainMenu:
		return "MainMenu";
	case GameMode::Editor:
		return "Editor";
	default:
		return "Unknown GameMode";
	}
}




// TODO: Reference additional headers your program requires here.
class Game {
public:
	Game() = default;
	Game(int width, int height);
	int run();
	void tick();


private:

	bool exitWindow{ false };
#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	bool showGamepad{ false };
#endif

	raylib::AudioDevice audioDevice{};


	SaveData saveData{};
	std::vector<Level> levels{};
	
	MainMenu mainMenu{ saveData, levels };

	Editor editor{};

	GameplayController gameplayController{};

	GameMode gameMode{ GameMode::MainMenu };
	void draw() const;     // Update and Draw one frame

	void processInputs(float deltaTime);

	InputManager input{};

	void update(float deltaTime);
	void handleEditorCommand(auto command);
	void handleMainMenuCommand(auto command);
	void handleGameplayCommand(auto command);

	int screenWidth = 1280;
	int screenHeight = 720;


};
