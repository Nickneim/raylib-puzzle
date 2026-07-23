#include "ui/MainMenu.hpp"

#include "core/Action.hpp"

void MainMenu::update(float deltaTime, Action action) {
	if (activeMenuScreen == MenuScreen::MainMenu) {
		const int optionsCount = static_cast<int>(MainMenuOption::Count);
		if (action == Action::MoveUp) {
			selectedOptionIndex -= 1;
			if (selectedOptionIndex == static_cast<int>(MainMenuOption::Continue) && isValidLevelIndex()) {
				selectedOptionIndex -= 1; // Skip Continue option if no level has been started
			}
			selectedOptionIndex = (selectedOptionIndex + optionsCount) % optionsCount;
			menuMoveSound.Play();
		}
		else if (action == Action::MoveDown) {
			selectedOptionIndex += 1;
			if (selectedOptionIndex == static_cast<int>(MainMenuOption::Continue) && isValidLevelIndex()) {
				selectedOptionIndex += 1; // Skip Continue option if no level has been started
			}
			selectedOptionIndex = (selectedOptionIndex + optionsCount) % optionsCount;
			menuMoveSound.Play();

		}
		else if (action == Action::Interact) {
			const auto selectedOption = static_cast<MainMenuOption>(selectedOptionIndex);

			if (selectedOption == MainMenuOption::StartNewGame) {
				gameStartSound.Play();
				pendingCommand = MenuCommand{ MenuCommandType::StartNewGame };
			}
			else if (selectedOption == MainMenuOption::Continue) {
				menuSelectSound.Play();
				pendingCommand = MenuCommand{ MenuCommandType::ContinueGame };
			}
			else if (selectedOption == MainMenuOption::LevelSelect) {
				menuSelectSound.Play();
				if (isValidLevelIndex()) {
					selectedOptionIndex = 0;
				}
				else {
					selectedOptionIndex = saveData.currentLevelIndex;
				}
				activeMenuScreen = MenuScreen::LevelSelect;
			}
			else if (selectedOption == MainMenuOption::ResetLevelProgress) {
				menuSelectSound.Play();
				pendingCommand = MenuCommand{ MenuCommandType::ResetLevelProgress };
			}
#if not defined(__EMSCRIPTEN__)
			else if (selectedOption == MainMenuOption::Exit) {
				menuSelectSound.Play();
				pendingCommand = MenuCommand{ MenuCommandType::Quit };
			}
#endif
		}
		else if (action == Action::Pause) {
			if (isValidLevelIndex()) {
				menuMoveSound.Play();
				pendingCommand = MenuCommand{ MenuCommandType::ContinueGame };
			}
		}
	}
	else if (activeMenuScreen == MenuScreen::LevelSelect) {
		const int optionsCount{ static_cast<int>(levels.size()) + 1 }; // +1 for the "Return to Main Menu" option
		if (action == Action::MoveUp) {
			menuMoveSound.Play();
			selectedOptionIndex = (selectedOptionIndex + optionsCount - 1) % optionsCount;
		}
		else if (action == Action::MoveDown) {
			menuMoveSound.Play();
			selectedOptionIndex = (selectedOptionIndex + optionsCount + 1) % optionsCount;
		}
		else if (action == Action::Interact) {
			menuSelectSound.Play();
			if (selectedOptionIndex >= 0 && selectedOptionIndex < static_cast<int>(levels.size())) {
				pendingCommand = MenuCommand{ MenuCommandType::LoadLevel, selectedOptionIndex };
			}
			else {
				activeMenuScreen = MenuScreen::MainMenu;
				selectedOptionIndex = static_cast<int>(MainMenuOption::LevelSelect);
			}
		}
		else if (action == Action::Undo || action == Action::Pause) {
			menuMoveSound.Play();
			activeMenuScreen = MenuScreen::MainMenu;
			selectedOptionIndex = static_cast<int>(MainMenuOption::LevelSelect);
		}
	}

}

bool MainMenu::isValidLevelIndex() const
{
	return saveData.currentLevelIndex < 0 || saveData.currentLevelIndex >= static_cast<int>(levels.size());
}

void MainMenu::draw() const
{
	ClearBackground(BLACK);

	
	// We start drawing the menu options based on ScreenHeight so that the menu is vertically centered, and we space the options 50 pixels apart
	int y = raylib::Window::GetHeight() / 2 - (MAX_VISIBLE_OPTIONS / 2 * 50);

	if (activeMenuScreen == MenuScreen::MainMenu) {
		constexpr int optionsCount = static_cast<int>(MainMenuOption::Count);
		for (int i = 0; i < optionsCount; ++i) {
			auto option{ static_cast<MainMenuOption>(i) };
			std::string optionText{ mainMenuOptionToString(option) };

			int x = raylib::Window::GetWidth() / 2 - raylib::MeasureText(optionText, 40) / 2;
			if (option == MainMenuOption::Continue && isValidLevelIndex()) {
				raylib::DrawText(optionText, x, y, 40, DARKGRAY);
			}
			else {
				raylib::DrawText(optionText, x, y, 40, selectedOptionIndex == i ? GREEN : WHITE);
			}
			y += 50;
		}
	}
	else if (activeMenuScreen == MenuScreen::LevelSelect) {
		// We only draw up to MAX_VISIBLE_OPTIONS level options, but we still want to allow selection of all levels, so we clamp the selectedOptionIndex to the visible range for coloring purposes
		int firstVisibleOptionIndex = std::max(0, std::min(static_cast<int>(levels.size()) - MAX_VISIBLE_OPTIONS, selectedOptionIndex - MAX_VISIBLE_OPTIONS / 2));

		for (int i = 0; i < std::min(static_cast<int>(levels.size()) - firstVisibleOptionIndex, MAX_VISIBLE_OPTIONS); ++i) {
			int levelIndex = firstVisibleOptionIndex + i;
			std::string optionText{ levels[levelIndex].name };
			if (saveData.levelIsSolved(levelIndex)) {
				optionText += " (Solved)";
			}
			int x = raylib::Window::GetWidth() / 2 - raylib::MeasureText(optionText, 40) / 2;
			raylib::DrawText(optionText, x, y, 40, selectedOptionIndex == levelIndex ? GREEN : WHITE);
			y += 50;
		}
		// We draw the return to main menu option if we are scrolled down far enough that the last level option is visible
		if (firstVisibleOptionIndex + MAX_VISIBLE_OPTIONS >= static_cast<int>(levels.size())) {
			std::string optionText{ "Return to Main Menu" };
			int x = raylib::Window::GetWidth() / 2 - raylib::MeasureText(optionText.c_str(), 40) / 2;
			raylib::DrawText(optionText, x, y, 40, selectedOptionIndex == static_cast<int>(levels.size()) ? GREEN : WHITE);
		}
	}
}
