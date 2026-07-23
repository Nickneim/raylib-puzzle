#include "core/Game.hpp"

#include <filesystem>
#include <format>

#include "core/Constants.hpp"
#include "ui/DebugDrawer.hpp"

Game::Game(int width, int height) : screenWidth(width), screenHeight(height)
{
}


static void MainLoop(void* arg)
{
	static_cast<Game*>(arg)->tick();
}

#if defined(__EMSCRIPTEN__)
EM_JS(bool, IsMobileBrowser, (), {
    const hasTouch = navigator.maxTouchPoints > 0;
    const isMobileUA = /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent);
    return hasTouch && isMobileUA;
});
//static bool IsMobileBrowser() {
//	return emscripten_run_script_int(
//		"navigator.maxTouchPoints > 0 && "
//		"/Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent) ? 1 : 0"
//	) == 1;
//}
#endif

int Game::run()
{
	raylib::Window window { screenWidth, screenHeight, "raylib-cpp [core] example - basic window" };

#if defined(__EMSCRIPTEN__)
	showGamepad = IsMobileBrowser();  // call once at startup
	// in your game loop draw:
	if (showGamepad) {
		input.initMobileGamepad();
	}
#endif

#if defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	showGamepad = true;
#endif
	levels = LevelLoader::loadLevelsFromFile("resources/levels.txt");

	saveData.setLevelAmount(static_cast<int>(levels.size()));
	saveData.load();

	// We find out what the first not-solved level was
	for (int i{ 0 }; i < static_cast<int>(levels.size()); ++i) {
		if (!saveData.levelIsSolved(i)) {
			saveData.currentLevelIndex = i;
			break;
		}
	}

	if (saveData.currentLevelIndex == 0) {
		saveData.currentLevelIndex = -1; // Start at -1 so that "Continue" option in main menu is disabled until a level is started
	}
	else {
		gameplayController.loadLevel(levels[saveData.currentLevelIndex]);
	}
	window.SetExitKey(KEY_NULL);


#if defined(__EMSCRIPTEN__)
		emscripten_set_main_loop_arg(MainLoop, this, 0, 1);
#else
        window.SetTargetFPS(60);
        //--------------------------------------------------------------------------------------
		
        // Main game loop
        while (!window.ShouldClose() && !exitWindow)
        {
            tick();
        }
#endif

        return 0;
    }

void Game::tick() {
	float deltaTime = raylib::Window::GetFrameTime();
    processInputs(deltaTime);
	update(deltaTime);
    draw();
}



void Game::processInputs(float deltaTime) {
	input.processInputs(deltaTime);
}




void Game::handleMainMenuCommand(auto command) {
	if (command.type == MenuCommandType::StartNewGame) {
		gameMode = GameMode::Gameplay;
		saveData.currentLevelIndex = 0;
		gameplayController.loadLevel(levels[saveData.currentLevelIndex]);
	}

	if (command.type == MenuCommandType::ContinueGame) {
		gameMode = GameMode::Gameplay;
		gameplayController.resume();
	}

	if (command.type == MenuCommandType::ResetLevelProgress) {
#if defined(FORCE_ENABLE_MOBILE_GAMEPAD)
		input.initMobileGamepad();
#else
		saveData.currentLevelIndex = -1;
		saveData.resetLevelProgress();
		saveData.save();
#endif
	}

	if (command.type == MenuCommandType::Quit) {
		exitWindow = true;
	}

	if (command.type == MenuCommandType::LoadLevel) {
		const int levelIndex = command.levelIndex;
		gameMode = GameMode::Gameplay;
		saveData.currentLevelIndex = levelIndex;
		gameplayController.loadLevel(levels[saveData.currentLevelIndex]);
	}
}

void Game::handleEditorCommand(auto command)
{

	if (command.type == EditorCommandType::OpenMainMenu) {
		gameMode = GameMode::MainMenu;
	}
	if (command.type == EditorCommandType::ReturnToGameplay) {
		gameMode = GameMode::Gameplay;
	}
}


void Game::handleGameplayCommand(auto command)
{
	if (command.type == GameplayCommandType::MarkLevelAsComplete) {
		if (saveData.currentLevelIndex + 1 < static_cast<int>(levels.size())) {
			if (!saveData.levelIsSolved(saveData.currentLevelIndex)) {
				saveData.markLevelAsSolved(saveData.currentLevelIndex);
				saveData.save();
			}
		}
	}
	if (command.type == GameplayCommandType::GoToNextLevel) {
		if (saveData.currentLevelIndex + 1 < static_cast<int>(levels.size())) {
			saveData.currentLevelIndex += 1;
			gameplayController.loadLevel(levels[saveData.currentLevelIndex]);
		}
		else {
			gameMode = GameMode::MainMenu;
		}
	}
	else if (command.type == GameplayCommandType::OpenEditor) {
		gameMode = GameMode::Editor;
		editor.loadLevel(gameplayController.level);
	}
	else if (command.type == GameplayCommandType::OpenMainMenu) {
		gameMode = GameMode::MainMenu;
	}
}

void Game::update(float deltaTime) {
	auto action = input.getAction();

	if (gameMode == GameMode::MainMenu) {
		mainMenu.update(deltaTime, action);
		handleMainMenuCommand(mainMenu.consumePendingCommand());
	}
	else if (gameMode == GameMode::Gameplay) {
		gameplayController.update(deltaTime, action);
		handleGameplayCommand(gameplayController.consumePendingCommand());
	}
	else if (gameMode == GameMode::Editor) {
		editor.update(deltaTime, action);
		handleEditorCommand(editor.consumePendingCommand());
	}
}


void Game::draw() const
{
    ClearBackground(RAYWHITE);

	auto& debugDrawer = DebugDrawer::get();

	debugDrawer.beginFrame();

	if (gameMode == GameMode::MainMenu) {
		mainMenu.draw();
	}
	else if (gameMode == GameMode::Gameplay) {
		gameplayController.draw();		
	}
	else if (gameMode == GameMode::Editor) {
		editor.draw();
	}

#if defined(__EMSCRIPTEN__) || defined(FORCE_ENABLE_MOBILE_GAMEPAD)
	if (showGamepad) {
		input.drawMobileGamepad();
	}
#endif

	debugDrawer.draw(std::format("Current level index: {}", saveData.currentLevelIndex));
	debugDrawer.draw(std::format("Game mode: {}", gameModeToString(gameMode)));
	debugDrawer.draw(std::format("Resolution: {}x{}", raylib::Window::GetWidth(), raylib::Window::GetHeight()));
	DrawFPS(raylib::Window::GetWidth() - 100, 10);

    EndDrawing();
}