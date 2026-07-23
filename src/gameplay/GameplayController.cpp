#include "gameplay/GameplayController.hpp"

#include <format>

#include "core/Constants.hpp"
#include "gameplay/GameplayLogic.hpp"
#include "gameplay/GameplayRenderer.hpp"
#include "ui/DebugDrawer.hpp"


void GameplayController::loadLevel(const Level& newLevel)
{
	dispatcher.dispatch(NewTurnEvent{});
	level = newLevel;
	currentState = newLevel.initialState;
	undoStack = std::stack<GameState>{};
	GameplayRenderer::updateCamera(camera, level);
	gameplayState = GameplayState::Playing;
	startTransition(TransitionState::FadingIn);
}

void GameplayController::updateWinState()
{
	if (GameplayLogic::isWinState(level.grid, currentState)) {
		if (gameplayState == GameplayState::Playing) {
			dispatcher.dispatch(LevelWinEvent{});
			pendingCommand = { GameplayCommandType::MarkLevelAsComplete };
		}
		gameplayState = GameplayState::LevelComplete;
	}
	else {
		gameplayState = GameplayState::Playing;
	}
}

void GameplayController::saveState(const GameState& state) {
	undoStack.push(state);
}

void GameplayController::undo() {
	if (undoStack.empty()) {
		return;
	}
	GameState previousState = undoStack.top();
	undoStack.pop();
	currentState = previousState;
}


void GameplayController::restart() {
	if (currentState != level.initialState) {
		saveState(currentState);
		currentState = level.initialState;
	}
}

void GameplayController::handlePlaying(float deltaTime, Action action) {
	if (transitionState != TransitionState::None) {
		if (transitionIsDone()) {
			gameplayState = GameplayState::Playing;
			stopTransition();
		}
		return;
	}

	if (action == Action::Editor) {
		pendingCommand = { GameplayCommandType::OpenEditor };
	}
	else if (action == Action::Undo) {
		undo();
	}
	else if (action == Action::Restart) {
		if (currentState != level.initialState) {
			saveState(currentState);
			currentState = level.initialState;
		}
	}
	else if (action == Action::None) {
		// pass
	}
	else if (action == Action::Pause) {
		pendingCommand = { GameplayCommandType::OpenMainMenu };
	}
	else {
		auto result = GameplayLogic::applyAction(level.grid, currentState, action);
		if (result.changed) {
			saveState(currentState);
			for (const Event& event : result.events)
			{
				dispatcher.dispatch(event);
			}

			currentState = std::move(result.newState);

		}
	}
}

void GameplayController::resume()
{
	if (GameplayLogic::isWinState(level.grid, currentState)) {
		gameplayState = GameplayState::LevelComplete;
	}
	else {
		gameplayState = GameplayState::Playing;
	}
	startTransition(TransitionState::FadingIn);
}

void GameplayController::handleLevelComplete(float deltaTime, Action action) {
	if (transitionState == TransitionState::FadingOut) {
		if (transitionIsDone()) {
			pendingCommand = { GameplayCommandType::GoToNextLevel };
		}
		return;
	}

	if (transitionState == TransitionState::FadingIn) {
		if (transitionIsDone()) {
			stopTransition();
		}
		return;
	}

	// transitionState == TransitionState::None
	if (action == Action::Undo) {
		undo();
	}
	else if (action == Action::Restart) {
		if (currentState != level.initialState) {
			saveState(currentState);
			currentState = level.initialState;
		}
	}
	else if (action == Action::Pause) {
		pendingCommand = { GameplayCommandType::OpenMainMenu };
	}
	else if (action == Action::None) {
		// pass
	}
	else {
		startTransition(TransitionState::FadingOut);
	}
}


void GameplayController::update(float deltaTime, Action action) {

	animationSystem.update(deltaTime);

	if (gameplayState == GameplayState::Playing) {
		handlePlaying(deltaTime, action);
	}
	else if (gameplayState == GameplayState::LevelComplete) {
		handleLevelComplete(deltaTime, action);
	}

	if (transitionState != TransitionState::None) {
		transitionTimer += deltaTime;
	}
	else {
		updateWinState();
	}
}




void GameplayController::startTransition(TransitionState newTransitionState)
{
	transitionState = newTransitionState;
	transitionTimer = 0.0f;
}

void GameplayController::stopTransition() {
	transitionState = TransitionState::None;
	transitionTimer = 0.0f;
}

bool GameplayController::transitionIsDone() const {
	return (transitionState == TransitionState::FadingOut && transitionTimer >= TRANSITION_FADING_OUT_DURATION) ||
		(transitionState == TransitionState::FadingIn && transitionTimer >= TRANSITION_FADING_IN_DURATION) ||
		(transitionState == TransitionState::None);
}

void GameplayController::draw() const
{
	GameplayRenderer::draw(camera, level, currentState, animationSystem);

	if (gameplayState == GameplayState::LevelComplete) {
		// We center the "Level Complete" text based on the screen width and height and the width of the text
		DrawText("Level Complete!", raylib::Window::GetWidth() / 2 - MeasureText("Level Complete!", 40) / 2, raylib::Window::GetHeight() / 2 - 20, 40, BLACK);
	}


	if (transitionState == TransitionState::FadingOut) {
		float transitionAlpha = std::min(transitionTimer / TRANSITION_FADING_OUT_DURATION, 1.0f);
		DrawRectangle(0, 0, raylib::Window::GetWidth(), raylib::Window::GetHeight(), Fade(BLACK, transitionAlpha));
	}
	else if (transitionState == TransitionState::FadingIn) {
		float transitionAlpha = 1.0f - std::min(transitionTimer / TRANSITION_FADING_OUT_DURATION, 1.0f);
		DrawRectangle(0, 0, raylib::Window::GetWidth(), raylib::Window::GetHeight(), Fade(BLACK, transitionAlpha));
	}

	auto& debugDrawer = DebugDrawer::get();
	debugDrawer.draw(std::format("Player position: ({}, {})", currentState.player.position.x, currentState.player.position.y));
	debugDrawer.draw(std::format("Undo stack size: {}", undoStack.size()));
	debugDrawer.draw(std::format("Transition state: {}", transitionStateToString(transitionState)));
	debugDrawer.draw(std::format("Level name: {}", level.name));
	debugDrawer.draw(std::format("Level author: {}", level.author));
}


