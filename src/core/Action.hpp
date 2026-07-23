#pragma once
#include <string_view>

enum class Action {
	None,
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight,
	Undo,
	Restart,
	Pause,
	Interact,
	Editor,
	LMB,
	RMB
};

inline std::string_view actionToString(Action action) {
	switch (action) {
	case Action::None:
		return "None";
	case Action::MoveUp:
		return "MoveUp";
	case Action::MoveDown:
		return "MoveDown";
	case Action::MoveLeft:
		return "MoveLeft";
	case Action::MoveRight:
		return "MoveRight";
	case Action::Undo:
		return "Undo";
	case Action::Restart:
		return "Restart";
	case Action::Pause:
		return "Pause";
	case Action::Interact:
		return "Interact";
	case Action::Editor:
		return "Editor";
	case Action::LMB:
		return "LMB";
	case Action::RMB:
		return "RMB";
	default:
		return "Unknown";
	}
}