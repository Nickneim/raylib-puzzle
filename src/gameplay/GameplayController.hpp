#pragma once

#include <optional>
#include <stack>
#include <string_view>

#include "raylib-cpp.hpp"

#include "core/Action.hpp"
#include "core/GameState.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/EventDispatcher.hpp"
#include "systems/SoundSystem.hpp"
#include "world/Level.hpp"

enum class GameplayState {
	Playing,
	Paused,
	LevelComplete
};

inline std::string_view gameplayStateToString(GameplayState state) {
	switch (state) {
	case GameplayState::Playing:
		return "Playing";
	case GameplayState::Paused:
		return "Paused";
	case GameplayState::LevelComplete:
		return "LevelComplete";
	default:
		return "Unknown GameplayState";
	}
}

enum class TransitionState {
	None,
	FadingOut,
	FadingIn
};

inline std::string_view transitionStateToString(TransitionState state) {
	switch (state) {
	case TransitionState::None:
		return "None";
	case TransitionState::FadingOut:
		return "FadingOut";
	case TransitionState::FadingIn:
		return "FadingIn";
	default:
		return "Unknown TransitionState";
	}
}

constexpr float TRANSITION_FADING_OUT_DURATION{ 0.5f };
constexpr float TRANSITION_FADING_IN_DURATION{ 0.5f };

enum class GameplayCommandType {
	None,
	OpenMainMenu,
	OpenEditor,
	MarkLevelAsComplete,
	GoToNextLevel
};

struct GameplayCommand {
	GameplayCommandType type{ GameplayCommandType::None };
};


class GameplayController {
public:
	void handlePlaying(float deltaTime, Action action);
	void update(float deltaTime, Action action);

	void draw() const;
	Level level{};

	GameState currentState{};

	std::stack<GameState> undoStack{};
	void loadLevel(const Level& level);
	GameplayCommand consumePendingCommand() {
		GameplayCommand command = pendingCommand;
		pendingCommand = GameplayCommand{};
		return command;
	}

	void resume();
private:

	AnimationSystem animationSystem{};
	SoundSystem soundSystem{};
	EventDispatcher dispatcher{ .animationSystem = animationSystem, .soundSystem = soundSystem };

	GameplayCommand pendingCommand;

	raylib::Camera2D camera{};

	GameplayState gameplayState{ GameplayState::Playing };
	TransitionState transitionState{ TransitionState::None };
	float transitionTimer{ 0.0f };
	bool transitionIsDone() const;

	void handleLevelComplete(float deltaTime, Action action);

	void startTransition(TransitionState newTransitionState);
	void stopTransition();

	void updateWinState();

	void saveState(const GameState& state);
	void undo();

	void restart();

};
